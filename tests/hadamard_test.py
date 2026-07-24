from scipy.linalg import hadamard
import numpy as np
import math

# function definition to compute magnitude o f the vector
def magnitude(vector): 
    return math.sqrt(sum(pow(element, 2) for element in vector))

H = hadamard(8) / np.sqrt(8)
H1 = hadamard(4) / 2

x = np.array([1,2,3,4,5,6,7,8])
x1 = np.array([3, 1, 7, 9])



print(H @ x)
print(H1 @ x1)

print(magnitude(x1))
print(magnitude(H1 @ x1))

I = np.eye(8)
e = np.array([1, 1, 1, 1, 1, 1, 1, 1])
M = e[:, None] @ e[None, :]
H1 = I - 0.25 * M

x2 = [2, 5, 3, 0, 1, 6, 4, 7]
print(H1 @ x2)

print(magnitude(x2))
print(magnitude(H1 @ x2))

I = np.eye(4)
e = np.array([1, 1, 1, 1])
M = e[:, None] @ e[None, :]
H1 = I - 0.5 * M

x2 = [0.5, 0.246, 1.3, 3.57]
print(H1 @ x2)

print(magnitude(x2))
print(magnitude(H1 @ x2))