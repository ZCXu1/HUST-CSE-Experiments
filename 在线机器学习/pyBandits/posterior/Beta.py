# -*- coding: utf-8 -*-
'''Beta posterior for binary bandits'''

__author__ = "Olivier Cappé, Aurélien Garivier, Emilie Kaufmann"
__version__ = "$Revision: 1.5 $"

from posterior.Posterior import Posterior

from random import betavariate
from scipy.special import btdtri

class Beta(Posterior):
    """Manipulate posteriors of Bernoulli/Beta experiments.
    """
    def __init__(self, a=1, b=1):
        self.a = a
        self.b = b
        
    def reset(self, a=0, b=0):
        if a==0:
            a = self.a
        if b==0:
            b = self.b
        self.N = [a, b]

    def update(self, obs):
        self.N[int(obs)] += 1
        
    def sample(self):
        return betavariate(self.N[1], self.N[0])

    def quantile(self, p):
        return btdtri(self.N[1], self.N[0], p) # Bug: do not call btdtri with (0.5,0.5,0.5) in scipy < 0.9
