import random
import scipy.optimize


def lennard_jones(x):
	E = 0
	m = len(x)
	for i in range(0, m, 3):
		for j in range(i + 3, m, 3):
			dx = x[i] - x[j]
			dy = x[i + 1] - x[j + 1]
			dz = x[i + 2] - x[j + 2]
			r2 = dx**2 + dy**2 + dz**2
			r2inv = 1 / r2
			r6inv = r2inv * r2inv * r2inv
			r12inv = r6inv * r6inv
			E += r12inv - r6inv
	return 4 * E


def compute_jacobian(x):
	m = len(x)
	F = [0] * m
	for i in range(0, m, 3):
		for j in range(i + 3, m, 3):
			dx = x[i] - x[j]
			dy = x[i + 1] - x[j + 1]
			dz = x[i + 2] - x[j + 2]
			r2 = dx**2 + dy**2 + dz**2
			r2inv = 1 / r2
			r4inv = r2inv * r2inv
			r8inv = r4inv * r4inv
			r14inv = r8inv * r4inv * r2inv
			f = 24 * r8inv - 48 * r14inv
			fx = dx * f
			fy = dy * f
			fz = dz * f
			F[i] += fx
			F[i + 1] += fy
			F[i + 2] += fz
			F[j] -= fx
			F[j + 1] -= fy
			F[j + 2] -= fz
	return F

x2 = [
    0.0000000000,
    0.0000000000,
    0.0000000000,
    1.1224620483,
    0.0000000000,
    0.0000000000,
]

x3 = [
    0.4391356726,
    0.1106588251,
    -0.4635601962,
    -0.5185079933,
    0.3850176090,
    0.0537084789,
    0.0793723207,
    -0.4956764341,
    0.4098517173,
]

x5 = [
    -0.2604720088,
    0.7363147287,
    0.4727061929,
    0.2604716550,
    -0.7363150782,
    -0.4727063011,
    -0.4144908003,
    -0.3652598516,
    0.3405559620,
    -0.1944131041,
    0.2843471802,
    -0.5500413671,
    0.6089042582,
    0.0809130209,
    0.2094855133,
]

if __name__ == "__main__":
    print(lennard_jones(x2), lennard_jones(x3), lennard_jones(x5))

    random.seed(12345)
    n = 38
    for t in range(10):
        x0 = [random.uniform(-1, 1) for i in range(3 * n)]
        res = scipy.optimize.minimize(lennard_jones, x0, method='CG', jac=compute_jacobian)
        print(t, res.message, res.fun, res.x)
