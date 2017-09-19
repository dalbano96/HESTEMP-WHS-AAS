# Import
import csv

def csv_load(filename):
    f = open(filename,'r')
    aline = f.read()
    f.close()
