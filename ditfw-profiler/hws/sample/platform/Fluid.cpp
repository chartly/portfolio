/*****************************************************************************/
/*

CS391 Project 3 Profiler Scenario
This is adapted from Cody Pritchard's PHY350/399 Fluid Simulation Work

This sample provides a nice, juicy, CPU intensive application to test
your profiler on for project 3.

*/
/*****************************************************************************/

#include "Fluid.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <xmmintrin.h>

using namespace DirectX;

inline XMVECTOR XMLoadFloat2Alternate
(
	const XMFLOAT2* pSource
)
{
	return _mm_loadl_pi(_mm_setzero_ps(), reinterpret_cast<const __m64 *>(pSource));
}

inline void XMStoreFloat2Alternate
(
	XMFLOAT2* pDestination, 
	FXMVECTOR  V
)
{
	return _mm_storel_pi(reinterpret_cast<__m64 *>(pDestination), V);
}

inline float XMSqrtApprox
(
	float X
)
{
	return _mm_cvtss_f32(_mm_rcp_ss(_mm_rsqrt_ss(_mm_set_ss(X))));
}

using namespace std;

// Zero the fluid simulation member variables for sanity
Fluid::Fluid() {
	step = 0;
	paused = false;
	pause_step = 0xFFFFFFFF;

	width = 0;
	height = 0;
	grid_w = 0;
	grid_h = 0;

	num_particles = 0;
	pos = NULL;
	vel = NULL;
	acc = NULL;
	density = NULL;
	pressure = NULL;
	gridindices = NULL;
	gridoffsets = NULL;
	num_neighbors = 0;
	// If this value is too small, ExpandNeighbors will fix it
	neighbors_capacity = 256 * 1024;
	neighbors = new FluidNeighborRecord[ neighbors_capacity ];

	// Precompute kernel coefficients
	// See "Particle-Based Fluid Simulation for Interactive Applications"
	// "Poly 6" Kernel - Used for Density
	poly6_coef = 315.0f / (64.0f * XM_PI * pow(FluidSmoothLen, 9));
	// Gradient of the "Spikey" Kernel - Used for Pressure
	grad_spiky_coef = -45.0f / (XM_PI * pow(FluidSmoothLen, 6));
	// Laplacian of the "Viscosity" Kernel - Used for Viscosity
	lap_vis_coef = 45.0f / (XM_PI * pow(FluidSmoothLen, 6));
}

// Destructor
Fluid::~Fluid() {
	Clear();
	delete[] gridoffsets; gridoffsets = NULL;
	num_neighbors = 0;
	neighbors_capacity = 0;
	delete[] neighbors; neighbors = NULL;
}

// Create the fluid simulation
// width/height is the simulation world maximum size
void Fluid::Create(float w, float h) {
	width = w;
	height = h;
	grid_w = (int)(width / FluidSmoothLen);
	grid_h = (int)(height / FluidSmoothLen);

	delete[] gridoffsets;
	gridoffsets = new FluidGridOffset[ (unsigned int)(grid_w * grid_h) ];
}

// Fill a region in the lower left with evenly spaced particles
void Fluid::Fill(float size) {
	Clear();

	unsigned int w = (unsigned int)(size / FluidInitialSpacing);
	// Allocate
	pos = new XMFLOAT2[ w * w ];
	vel = new XMFLOAT2[ w * w ];
	acc = new XMFLOAT2[ w * w ];
	density = new float[ w * w ];
	pressure = new float[ w * w ];
	gridindices = new unsigned int[ w * w ];

	// Populate
	for (unsigned int y = 0 ; y < w ; y++) {
		for (unsigned int x = 0 ; x < w ; x++) {
			pos[ y*w+x ] = XMFLOAT2(x * FluidInitialSpacing, Height() - y * FluidInitialSpacing);
			vel[ y*w+x ] = XMFLOAT2(0, 0);
			acc[ y*w+x ] = XMFLOAT2(0, 0);
			density[ y*w+x ] = 0;
			pressure[ y*w+x ] = 0;
			gridindices[ y*w+x ] = 0;
		}
	}
	num_particles = w * w;
}

// Remove all particles
void Fluid::Clear() {
	step = 0;
	num_particles = 0;
	delete[] pos; pos = NULL;
	delete[] vel; vel = NULL;
	delete[] acc; acc = NULL;
	delete[] density; density = NULL;
	delete[] pressure; pressure = NULL;
	delete[] gridindices; gridindices = NULL;
}

// Expand the Neighbors list if necessary
// This function is rarely called
__declspec(noinline) void Fluid::ExpandNeighbors() {
	// Double the size of the neighbors array because it is full
	neighbors_capacity *= 2;
	FluidNeighborRecord* new_neighbors = new FluidNeighborRecord[ neighbors_capacity ];
	memcpy( new_neighbors, neighbors, sizeof(FluidNeighborRecord) * num_neighbors );
	delete[] neighbors;
	neighbors = new_neighbors;
}

// Simulation Update
// Build the grid of neighbors
// Imagine an evenly space grid.  All of our neighbors will be
// in our cell and the 8 adjacent cells
void Fluid::UpdateGrid() {
	// Cell size is the smoothing length
	float invh = 1.0f / FluidSmoothLen;

	// Clear the offsets
	for (int O = 0 ; O < (grid_w * grid_h) ; O++) {
		gridoffsets[O].count = 0;
	}

	// Count the number of particles in each cell
	for (unsigned int P = 0 ; P < num_particles ; P++) {
		// Find where this particle is in the grid
		int p_gx = min(max((int)(pos[P].x * invh), 0), grid_w - 1);
		int p_gy = min(max((int)(pos[P].y * invh), 0), grid_h - 1);
		int cell = p_gy * grid_w + p_gx ;
		gridoffsets[ cell ].count++;
	}

	// Prefix sum all of the cells
	unsigned int sum = 0;
	for (int O = 0 ; O < (grid_w * grid_h) ; O++) {
		gridoffsets[O].offset = sum;
		sum += gridoffsets[O].count;
		gridoffsets[O].count = 0;
	}

	// Insert the particles into the grid
	for (unsigned int P = 0 ; P < num_particles ; P++) {
		// Find where this particle is in the grid
		int p_gx = min(max((int)(pos[P].x * invh), 0), grid_w - 1);
		int p_gy = min(max((int)(pos[P].y * invh), 0), grid_h - 1);
		int cell = p_gy * grid_w + p_gx ;
		gridindices[ gridoffsets[ cell ].offset + gridoffsets[ cell ].count ] = P;
		gridoffsets[ cell ].count++;
	}
}

// Simulation Update
// Build a list of neighbors (particles from adjacent grid locations) for every particle
void Fluid::GetNeighbors() {
	// Search radius is the smoothing length
	float h2 = FluidSmoothLen*FluidSmoothLen;
	float invh = 1.0f / FluidSmoothLen;

	num_neighbors = 0;

	for (unsigned int P = 0 ; P < num_particles ; P++) {
		// Find where this particle is in the grid
		int p_gx = min(max((int)(pos[P].x * invh), 0), grid_w - 1);
		int p_gy = min(max((int)(pos[P].y * invh), 0), grid_h - 1);
		int cell = p_gy * grid_w + p_gx ;
		XMFLOAT2 pos_P = pos[P];

		// For every adjacent grid cell (9 cells total for 2D)
		for (int d_gy = ((p_gy<1)?0:-1); d_gy <= ((p_gy<grid_h-1)?1:0); d_gy++) {
			for (int d_gx = ((p_gx<1)?0:-1); d_gx <= ((p_gx<grid_w-1)?1:0); d_gx++) {
				// Neighboring cell
				int n_cell = cell + d_gy * grid_w + d_gx; 

				// Loop over ever particle in the neighboring cell
				unsigned int* E = gridindices + gridoffsets[n_cell].offset;
				unsigned int* END = E + gridoffsets[n_cell].count;
				for ( ; E != END ; ++E) {
					unsigned int N = *E;
					// Only record particle "pairs" once
					if (P > N) {
						// Distance squared
						XMFLOAT2 d = XMFLOAT2(pos_P.x - pos[N].x, pos_P.y - pos[N].y);
						// Although this looks like a good candidate for XMVector2Dot
						// In practice it usually isn't.  This is such a small block
						// of math followed by a comparison that it isn't well suited
						// to SSE.  Perhaps the SSE4.1 PTEST instruction would be of use
						float distsq = d.x * d.x + d.y * d.y;

						// Check that the particle is within the smoothing length
						if (distsq < h2) {
							if (num_neighbors >= neighbors_capacity) {
								ExpandNeighbors();
							}
							// Record the ID of the two particles
							// And record the squared distance
							FluidNeighborRecord& record = neighbors[ num_neighbors ];
							record.p = P;
							record.n = N;
							record.distsq = distsq;
							num_neighbors++;
						}
					}
				}
			}
		}
	}
}

// Simulation Update
// Compute the density for each particle based on its neighbors within the smoothing length
void Fluid::ComputeDensity() {
	// Smoothing length squared
	float h2 = FluidSmoothLen*FluidSmoothLen;

	for (unsigned int P = 0 ; P < num_particles ; P++) {
		// This is r = 0
		density[P] = h2 * h2 * h2 * FluidWaterMass;
	}

	// foreach neighboring pair of particles
	for (unsigned int i = 0; i < num_neighbors ; i++) {
		unsigned int P = neighbors[i].p;
		unsigned int N = neighbors[i].n;
		
		// distance squared
		float r2 = neighbors[i].distsq;
		
		// Density is based on proximity and mass
		// Density is:
		// M_n * W(h, r)
		// Where the smoothing kernel is:
		// The the "Poly6" kernel
		float h2_r2 = h2 - r2;
		float dens = h2_r2*h2_r2*h2_r2;

		float P_mass = FluidWaterMass;
		float N_mass = FluidWaterMass;

		density[P] += N_mass * dens;
		density[N] += P_mass * dens;
	}

	// Approximate pressure as an ideal compressible gas
	// based on a spring eqation relating the rest density
	for (unsigned int P = 0 ; P < num_particles ; ++P) {
		density[P] *= poly6_coef;
		float density_ratio = density[P] / FluidRestDensity;
		pressure[P] = FluidStiff * max((density_ratio*density_ratio*density_ratio) - 1, 0.0f);
	}
}

// Simulation Update
// Perform a batch of sqrts to turn distance squared into distance
// Why is this noinline?  So that it shows up as a function to sampling profilers
__declspec(noinline) void Fluid::SqrtDist() {
	for (unsigned int i = 0; i < num_neighbors ; i++) {
		neighbors[i].distsq = XMSqrtApprox(neighbors[i].distsq);
	}
}

// Simulation Update
// Compute the forces based on the Navier-Stokes equations for laminer fluid flow
// Follows is lots more voodoo
void Fluid::ComputeForce() {
	float h = FluidSmoothLen;

	// foreach neighboring pair of particles
	for (unsigned int i = 0; i < num_neighbors ; i++) {
		unsigned int P = neighbors[i].p;
		unsigned int N = neighbors[i].n;

		// distance
		float r = neighbors[i].distsq;
		
		// Compute force due to pressure and viscosity
		float h_r = h - r;
		XMVECTOR pos_P = XMLoadFloat2Alternate(&pos[P]);
		XMVECTOR pos_N = XMLoadFloat2Alternate(&pos[N]);
		XMVECTOR diff = pos_N - pos_P;

		// Forces is dependant upon the average pressure and the inverse distance
		// Force due to pressure is:
		// 1/rho_p * 1/rho_n * Pavg * W(h, r)
		// Where the smoothing kernel is:
		// The gradient of the "Spikey" kernel
		XMVECTOR force = (0.5f * (pressure[P] + pressure[N])* grad_spiky_coef * h_r / r) * diff;

		// Viscosity is based on relative velocity
		// Viscosity is:
		// 1/rho_p * 1/rho_n * Vrel * mu * W(h, r)
		// Where the smoothing kernel is:
		// The laplacian of the "Viscosity" kernel
		XMVECTOR vel_P = XMLoadFloat2Alternate(&vel[P]);
		XMVECTOR vel_N = XMLoadFloat2Alternate(&vel[N]);
		force += ((FluidViscosity * lap_vis_coef) * (vel_N - vel_P));

		// Throw in the common (h-r) * 1/rho_p * 1/rho_n
		force *= h_r * 1.0f / (density[P] * density[N]);

		// Apply force - equal and opposite to both particles
		XMVECTOR acc_P = XMLoadFloat2Alternate(&acc[P]);
		XMVECTOR acc_N = XMLoadFloat2Alternate(&acc[N]);
		acc_P += FluidWaterMass * force;
		acc_N -= FluidWaterMass * force;
		XMStoreFloat2Alternate(&acc[P], acc_P);
		XMStoreFloat2Alternate(&acc[N], acc_N);
	}
}

// Simulation Update
// Integration
void Fluid::Integrate(float dt) {
	// Walls
	const XMVECTOR planes[4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{-1, 0, 0, width},
		{0, -1, 0, height}
	};

	const XMVECTOR gravity = XMVectorSet(0, 1, 0, 0);
	for (unsigned int P = 0 ; P < num_particles ; ++P) {
		XMVECTOR pos_P = XMLoadFloat2Alternate(&pos[P]);
		XMVECTOR vel_P = XMLoadFloat2Alternate(&vel[P]);
		XMVECTOR acc_P = XMLoadFloat2Alternate(&acc[P]);

		// Walls
		for (unsigned int i = 0 ; i < _countof(planes) ; i++) {
			//float dist = pos_P.x * planes[i].x + pos_P.y * planes[i].y + planes[i].z;
			//acc_P += min(dist, 0.0f) * -FluidStaticStiff * D3DXVECTOR2(planes[i].x, planes[i].y);
			XMVECTOR dist = XMPlaneDotCoord(planes[i], pos_P);
			acc_P += -FluidStaticStiff * XMVectorMin(dist, XMVectorZero()) * planes[i];
		}

		// Acceleration
		acc_P += gravity;

		// Integration - Euler-Cromer
		vel_P += dt * acc_P;
		pos_P += dt * vel_P;
		XMStoreFloat2Alternate(&pos[P], pos_P);
		XMStoreFloat2Alternate(&vel[P], vel_P);
		XMStoreFloat2Alternate(&acc[P], XMVectorZero());
	}
}

// Simulation Update
void Fluid::Update(float dt) {
	// Pause runs the simulation standing still for profiling
	if (paused || step == pause_step) { dt = 0.0f; }
	else { step++; }

	// Create neighbor information
	UpdateGrid();
	GetNeighbors();

	// Calculate the forces for all of the particles
	ComputeDensity();
	SqrtDist();
	ComputeForce();

	// And integrate
	Integrate(dt);
}
