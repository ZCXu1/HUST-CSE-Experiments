# -*- coding: utf-8 -*-
'''The Emipirical KL-UCB algorithm.
Reference: References: [Maillard, Munos & Stoltz - COLT, 2011], [Cappé, Garivier,  Maillard, Munos & Stoltz, 2012].
'''

__author__ = "Olivier Cappé, Aurélien Garivier"
__version__ = "$Revision: 1.8 $"


from math import log
import numpy as np

from kullback import maxEV
from policy.IndexPolicy import IndexPolicy

class KLempUCB(IndexPolicy):
    """Class that implements the KL-UCB non-parametric policy.
    """        

    def __init__(self, nbArms, maxReward=1.):
        self.c = 1
        self.nbArms = nbArms
        self.maxReward = maxReward
        self.nbDraws = dict()
        self.obs = dict() 

    def startGame(self):
        self.t = 1
        for arm in range(self.nbArms):
            self.nbDraws[arm] = 0
            self.obs[arm] = dict({self.maxReward:0})

    def computeIndex(self, arm):
        if self.nbDraws[arm]>0:
            return self.KLucb(self.obs[arm], self.c*log(self.t)/self.nbDraws[arm])
        else:
            return float('+infinity')

    def getReward(self, arm, reward):
        self.nbDraws[arm] += 1
        if reward in self.obs[arm]:
            self.obs[arm][reward] += 1
        else:
            self.obs[arm][reward] = 1
        self.t += 1

    def KLucb(self, obs, klMax):
        print(list(obs.values()))
        p = (np.array(list(obs.values())).astype(float))/sum(obs.values())
        v = np.array(obs.keys())
        #print "calling maxEV("+str(p)+", "+str(v)+", "+str(klMax)+")"
        q = maxEV(p, v, klMax)
        #q2 = kbp.maxEV(p, v, klMax)
        #if max(abs(q-q2))>1e-8:
        #    print "ERROR: for p="+str(p)+" ,v = "+str(v)+" and klMax = "+str(klMax)+" : "
        #    print "q = "+str(q)
        #    print "q2 = "+str(q2)
        #    print "_____________________________"
        #print "q = "+str(q)
        return(np.dot(q,v))
