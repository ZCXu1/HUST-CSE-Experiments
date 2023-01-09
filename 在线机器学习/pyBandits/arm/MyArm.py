import random
from arm.Arm import Arm
import pandas

class MyArm(Arm):
    def __init__(self, idx):
        self.idx=idx
        self.csv=pandas.read_csv("data\\YourDataset.csv")
        l=len(self.csv[self.csv.columns[idx]])
        self.l=l

        sum=0.
        for i in range(l):
                sum+=self.csv[self.csv.columns[idx]][i]

        self.expectation= sum/l
        self.expectation/=l
        #print(self.expectation)

    def draw(self):
        i=random.randint(0,len(self.csv[self.csv.columns[self.idx]])-1)
        result=self.csv[self.csv.columns[self.idx]][i]/self.l
        #print(result)
        #print(i)
        return result






a=MyArm(1)
a.draw()