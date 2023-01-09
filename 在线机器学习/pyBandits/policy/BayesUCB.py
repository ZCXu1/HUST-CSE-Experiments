# -*- coding: utf-8 -*-
'''The Bayes-UCB policy.
 Reference: [Kaufmann, Cappé & Garivier - AISTATS, 2012]'''

__author__ = "Olivier Cappé, Aurélien Garivier, Emilie Kaufmann"
__version__ = "$Revision: 1.7 $"

from random import choice

from posterior import Posterior
from policy.IndexPolicy import IndexPolicy

class BayesUCB(IndexPolicy):
    """The Bayes-UCB.
      Reference: [Kaufmann, Cappé & Garivier - AISTATS, 2012].
    """

    def __init__(self, nbArms, posterior):
        self.nbArms = nbArms
        self.posterior = dict()
        for arm in range(self.nbArms):
            self.posterior[arm] = posterior()

    def startGame(self):
        self.t = 1;
        for arm in range(self.nbArms):
            self.posterior[arm].reset()

    def getReward(self, arm, reward):
        self.posterior[arm].update(reward)
        self.t += 1
    
    def computeIndex(self, arm):
        return self.posterior[arm].quantile(1-1./self.t)
