# -*- coding: utf-8 -*-
"""
Plotter
"""
import matplotlib.pyplot as plt

error1x = [7, 1, 1, 7, 2, 1, 5]
error1y = [165, 50, 51, 180, 113, 34, 57]

error2x = [2, 2, 3, 3, 6, 1, 3]
error2y = [81, 81, 158, 158, 104, 49, 143]

error3x = [3, 2, 7, 4, 4]
error3y = [143, 96, 135, 145, 145]

error4x = [6, 2, 7, 1, 6, 2, 4]
error4y = [88, 113, 135, 49, 118, 112, 145]

error5x = [6, 6, 6, 1, 7, 7, 7, 1, 2, 3, 1, 6]
error5y = [88, 88, 73, 34, 150, 150, 135, 57, 96, 158, 49, 104]

error6x = [0, 7, 5, 6, 2, 5, 3, 2, 6, 1, 1, 5, 5, 6, 6, 7, 6]
error6y = [0, 165, 178, 88, 81, 41, 173, 111, 73, 34, 34, 56, 41, 88, 88, 165, 104]

error7x = [6, 2, 3, 0, 1, 1, 2, 2, 2, 7, 7, 7, 1, 4, 4, 4, 7, 6, 6, 5, 2, 2]
error7y = [73, 111, 175, 213, 275, 275, 81, 81, 96, 135, 135, 165, 34, 205, 205, 205, 180, 88, 88, 56, 276, 276]

error8x = [0, 3, 4, 3, 3, 3]
error8y = [243, 173, 220, 143, 83, 83]

error9x = [0, 1, 2, 3, 3, 1, 5]
error9y = [0, 51, 81, 175, 143, 34, 57]

error10x = [6, 6, 0, 0, 1, 1, 4, 4, 2, 3, 3]
error10y = [118, 118, 243, 213, 275, 275, 250, 235, 96, 83, 83]

errorx = error1x+error2x+error3x+error4x+error5x+error6x+error7x+error8x+error9x+error10x
errory = error1y+error2y+error3y+error4y+error5y+error6y+error7y+error8y+error9y+error10y



harderrorx = [error1x[-1],error2x[-1],error3x[-1],error4x[-1],error5x[-1],error6x[-1],error7x[-1],error8x[-1],error9x[-1],error10x[-1]]
harderrory = [error1y[-1],error2y[-1],error3y[-1],error4y[-1],error5y[-1],error6y[-1],error7y[-1],error8y[-1],error9y[-1],error10y[-1]]

plt.plot(errorx, errory, 'yo',label='Errors')
plt.plot(harderrorx, harderrory, 'ro',label='Hard Errors')
plt.title('Memory map of error sensitive locations')
plt.legend()
plt.show()