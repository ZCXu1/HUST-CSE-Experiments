# -*- coding: utf-8 -*-
'''The generic kl-UCB policy for one-parameter exponential distributions.
  Reference: [Garivier & cappé - COLT, 2011].'''

__author__ = "Olivier Cappé, Aurélien Garivier"
__version__ = "$Revision: 1.15 $"

from math import log

import kullback
from policy.IndexPolicy import IndexPolicy

class klUCB(IndexPolicy):
    """The generic kl-UCB policy for one-parameter exponential distributions.
      """
    def __init__(self, nbArms, amplitude=1., lower=0., klucb=kullback.klucbBern):
        self.c = 1.
        self.nbArms = nbArms
        self.amplitude = amplitude
        self.lower = lower
        self.nbDraws = dict()     #选中某个臂的次数
        self.cumReward = dict()
        self.klucb = klucb

    def startGame(self):
        self.t = 1
        for arm in range(self.nbArms):
            self.nbDraws[arm] = 0
            self.cumReward[arm] = 0.0

    def computeIndex(self, arm):
        if self.nbDraws[arm] == 0:
            return float('+infinity')
        else:
            return self.klucb(self.cumReward[arm] / self.nbDraws[arm], self.c * log(self.t) / self.nbDraws[arm], 1e-4) # Could adapt tolerance to the value of self.t

    def getReward(self, arm, reward):
        self.nbDraws[arm] += 1
        if reward != '-':
            self.cumReward[arm] += (float(reward) - self.lower) / self.amplitude
        self.t += 1
    # Debugging code
    #print "arm " + str(arm) + " receives " + str(reward)
    #print str(self.nbDraws[arm]) + " " + str(self.cumReward[arm])
