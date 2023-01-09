# -*- coding: utf-8 -*-
'''Bernoulli distributed arm.'''

__author__ = "Olivier Cappé, Aurélien Garivier"
__version__ = "$Revision: 1.6 $"

from random import random
from arm.Arm import Arm

class Bernoulli(Arm):
    """Bernoulli distributed arm."""

    def __init__(self, p):
        self.p = p
        self.expectation = p
        
    def draw(self):
        return float(random()<self.p)
