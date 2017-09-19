'''
Daryl Albano
CS 422
Linear Regression
'''

from numpy import *
from math import sqrt
from csv import reader
from random import seed
from random import randrange
import Tkinter, tkFileDialog
import numpy as np
import os

# Load a CSV file
def load_csv(filename):
    dataset = list()
    with open(filename, 'r') as file:
        next(file)
        csv_reader = reader(file)
        for row in csv_reader:
            if not row:
                continue
            dataset.append(row)
    return dataset

# Convert string column to float
def str_column_to_float(dataset, column):
    for row in dataset:
        row[column] = float(row[column].strip())

# Split a dataset into a train set and test set
def train_test_split(dataset, split):
    train = list()
    train_size = split * len(dataset)
    dataset_copy = list(dataset)
    while len(train) < train_size:
        index = randrange(len(dataset_copy))
        train.append(dataset_copy.pop(index))
    return train, dataset_copy

# Calculate root mean squared error
def rmse_metric(actual, predicted):
    sum_error = 0.0
    for i in range(len(actual)):
        prediction_error = predicted[i] - actual[i]
        sum_error += (prediction_error **2)
    mean_error = sum_error / float(len(actual))
    return sqrt(mean_error)

# Evaluate an algorithm using a train/test split
def evaluate_algorithm(dataset, algorithm):
    test_set = list()
    for row in dataset:
        row_copy = list(row)
        row_copy[-1] = None
        test_set.append(row_copy)
    predicted = algorithm(dataset, test_set)
    actual = [row[-1] for row in dataset]
    rmse = rmse_metric(actual, predicted)
    return rmse

# Return predicted values in order to graph regression model
def graph_algorithm(dataset, algorithm):
    test_set = list()
    for row in dataset:
        row_copy = list(row)
        row_copy[-1] = None
        test_set.append(row_copy)
    predicted = algorithm(dataset, test_set)
    return predicted

# Calculate the mean value of a list of numbers
def mean(values):
    return sum(values) / float(len(values))

# Calculate the variance of a list of numbers
def variance(values, mean):
    return sum([(x - mean)**2 for x in values])

# Calculate covariance between x and y
def covariance(x, mean_x, y, mean_y):
    covar = 0.0
    for i in range(len(x)):
        covar += (x[i] - mean_x) * (y[i] - mean_y)
    return covar

# Calculate coefficients
def coefficients(dataset):
    x = [row[0] for row in dataset]
    y = [row[1] for row in dataset]
    x_mean, y_mean = mean(x), mean(y)
    b1 = covariance(x, x_mean, y, y_mean) / variance(x, x_mean)
    b0 = y_mean - b1 * x_mean
    return [b0, b1]

# Simple linear regression algorithm
def simple_linear_regression(train, test):
    predictions = list()
    b0, b1 = coefficients(train)
    for row in test:
        yhat = b0 + b1 * row[0]
        predictions.append(yhat)
    return predictions

# Browse and select a file
root = Tkinter.Tk()
filename = tkFileDialog.askopenfilename()

# Load data into program
dataset = load_csv(filename)
for i in range(len(dataset[0])):
    str_column_to_float(dataset, i)

# Load data again as x and y values for graphing
dataIn = np.genfromtxt(filename, delimiter = ",")
x = dataIn[:, 0]
y = dataIn[:, 1]

# Begin algorithm
split = 0.1
rmse = evaluate_algorithm(dataset, simple_linear_regression)
print('RMSE: %.3f' % (rmse))

# Plot solution
from matplotlib.pyplot import *
points = graph_algorithm(dataset, simple_linear_regression)
plot(x, y, 'o', points, 'r')
filename_title = os.path.basename(filename)
title(filename_title)
xlabel('Data Entries')
ylabel('PM2.5 Concentration')
legend(['Measured', 'Predicted'])
xlim(0);
show()

'''
from scipy.interpolate import *
p1 = polyfit(x,y,1)
p2 = polyfit(x,y,2)
p3 = polyfit(x,y,3)
print(p1)
print(p2)
print(p3)

from matplotlib.pyplot import *
plot(x,y,'o')
xp = linspace(-2,6,100)
plot(xp,polyval(p1,xp),'r-')
plot(xp,polyval(p2,xp),'b--')
plot(xp,polyval(p3,xp),'m:')
yfit = p1[0] * x + p1[1]
yresid= y - yfit
SSresid = sum(pow(yresid,2))
SStotal = len(y) * var(y)
rsq = 1 - SSresid/SStotal
print(yfit)
print(y)
print(rsq)

from scipy.stats import *
slope,intercept,r_value,p_value,std_err = linregress(x,y)
print(pow(r_value,2))
print(p_value)
show()
'''
