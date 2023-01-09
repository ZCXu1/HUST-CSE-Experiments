# -*- coding: utf-8 -*-
'''Gaussian distributed arm.'''

__author__ = "Olivier Cappé, Aurélien Garivier"
__version__ = "$Revision: 1.4 $"

import random as rand
from Arm import Arm

class Gaussian(Arm):
    """Gaussian distributed arm."""
    def __init__(self, mu, sigma):
        self.sigma = sigma
        self.mu=mu
        self.expectation = mu
        
    def draw(self):
        return self.mu+self.sigma*rand.gauss(0,1)
