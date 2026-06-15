N = 20;
h = 1.0 / N;

Point(1) = {0, 0, 0, h};
Point(2) = {1, 0, 0, h};
Point(3) = {1, 1, 0, h};
Point(4) = {0, 1, 0, h};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Curve Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};

// Boundary IDs:
// 0 = x=0 external boundary.
// 1 = x=1 interface with Omega_2.
// 2 = y=0 external boundary.
// 3 = y=1 external boundary.
Physical Curve(0) = {4};
Physical Curve(1) = {2};
Physical Curve(2) = {1};
Physical Curve(3) = {3};
Physical Surface(10) = {1};

Mesh 2;
Save "mesh-problem-0.msh";
