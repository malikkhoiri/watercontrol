"""
Title: Very simple ADALINE network
Author: Stephen Sheridan (ITB) https://github.com/stephensheridan
Date: 09/03/2017
"""

import numpy as np
import matplotlib.pyplot as plt
import math

LEARNING_RATE = 0.01

# Step function
def step(x):
    if (x > 0):
        return 1
    else:
        return -1
    
"""
You can comment out either the first or second problem to see how the ADALINE network performs with
linearly separable and non linearly separable problems.
"""

# F I R S T   P R O B L E M - L O G I C A L   O R   L I N E A R
# input dataset representing the logical OR operator (including constant BIAS input of 1)
INPUTS = np.array([[-1,-1,-1,1],
                   [-1,-1,0,1],
                   [-1,-1,1,1],
                   [-1,0,-1,1],
                   [-1,0,0,1],
                   [-1,0,1,1],
                   [-1,1,-1,1],
                   [-1,1,0,1],
                   [-1,1,1,1],
                   [0,-1,-1,1],
                   [0,-1,0,1],
                   [0,-1,1,1],
                   [0,0,-1,1],
                   [0,0,0,1],
                   [0,0,1,1],
                   [0,1,-1,1],
                   [0,1,0,1],
                   [0,1,1,1],
                   [1,-1,-1,1],
                   [1,1,1,1]])

INPUTS1 = np.array([[1,-1,0,1],
                   [1,-1,1,1],
                   [1,0,-1,1],
                   [1,0,0,1],
                   [1,0,1,1],
                   [1,1,-1,1],
                   [1,1,0,1]])

# output dataset - Only output a -1 if both inputs are -1          
OUTPUTS = np.array([[-1,-1,-1,-1,-1,-1,-1,1,1,-1,-1,-1,-1,-1,-1,-1,1,1,-1,1]]).T


# S E C O N D   P R O B L E M - L O G I C A L   X O R - N O N   L I N E A R
# input dataset representing the logical OR operator (including constant BIAS input of 1)
#INPUTS = np.array([[-1,-1,1],
#                   [-1,1,1],
#                   [1,-1,1],
#                   [1,1,1] ])
# output dataset - Only output a -1 if both inputs are -1          
#OUTPUTS = np.array([[-1,1,1,-1]]).T

# seed random numbers to make calculation
# deterministic (just a good practice for testing)
np.random.seed(1)

# initialize weights randomly with mean 0
WEIGHTS = 2*np.random.random((4,1)) - 1
#WEIGHTS = 0*np.random.random((4,1))
print("Random Weights before training\r\n", WEIGHTS)

# Use this list to store the errors
errors=[]

# Training loop
i=1
for iter in range(100):

    for input_item,target in zip(INPUTS, OUTPUTS):
        
        # Feed this input forward and calculate the ADALINE output
        ADALINE_OUTPUT = (input_item[0]*WEIGHTS[0]) + (input_item[1]*WEIGHTS[1]) + (input_item[2]*WEIGHTS[2]) + (input_item[3]*WEIGHTS[3]) # Sigma wi.xi

        # Run ADALINE_OUTPUT through the step function
        ADALINE_OUTPUT = step(ADALINE_OUTPUT)   # net, y

        # Calculate the ERROR generated
        ERROR = target - ADALINE_OUTPUT # (t-y)
        
        # Store the ERROR
        errors.append(ERROR)
        
        # Update the weights based on the delta rule
        #print(i, ": ", WEIGHTS[0], ", ", WEIGHTS[1], ", ", WEIGHTS[2], ", ", WEIGHTS[3], "\r\n")
        WEIGHTS[0] = WEIGHTS[0] + LEARNING_RATE * ERROR * input_item[0] # w baru + learning rate*(t-y)*wi
        WEIGHTS[1] = WEIGHTS[1] + LEARNING_RATE * ERROR * input_item[1]
        WEIGHTS[2] = WEIGHTS[2] + LEARNING_RATE * ERROR * input_item[2]
        WEIGHTS[3] = WEIGHTS[3] + LEARNING_RATE * ERROR * input_item[3]
        i+=1


print("New Weights after training\r\n", WEIGHTS)
for input_item,desired in zip(INPUTS, OUTPUTS):
    # Feed this input forward and calculate the ADALINE output
    ADALINE_OUTPUT = (input_item[0]*WEIGHTS[0]) + (input_item[1]*WEIGHTS[1]) + (input_item[2]*WEIGHTS[2]) + (input_item[3]*WEIGHTS[3])

    # Run ADALINE_OUTPUT through the step function
    ADALINE_OUTPUT = np.around(ADALINE_OUTPUT, decimals=2)

    print("T", desired, "\tY ", step(ADALINE_OUTPUT))

    
print("\r\nTesting")
for input_item,desired in zip(INPUTS1, OUTPUTS):
    # Feed this input forward and calculate the ADALINE output
    ADALINE_OUTPUT = (input_item[0]*WEIGHTS[0]) + (input_item[1]*WEIGHTS[1]) + (input_item[2]*WEIGHTS[2]) + (input_item[3]*WEIGHTS[3])

    # Run ADALINE_OUTPUT through the step function
    ADALINE_OUTPUT = np.around(ADALINE_OUTPUT, decimals=2)

    print("Input", input_item, "\tActual ", ADALINE_OUTPUT)


# Plot the errors to see how we did during training
ax = plt.subplot(111)
ax.plot(errors, c='#aaaaff', label='Training Errors')
ax.set_xscale("log")
plt.title("ADALINE Errors (2,-2)")
plt.legend()
plt.xlabel('Error')
plt.ylabel('Value')
plt.show()
