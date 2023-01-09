# -*- coding: utf-8 -*-
'''The UCB policy for bounded bandits
  Reference: [Auer, Cesa-Bianchi & Fisher - Machine Learning, 2002], with constant
  set (optimally) to 1/2 rather than 2.
  
  Note that UCB is implemented as a special case of klUCB for the divergence 
  corresponding to the Gaussian distributions, see [Garivier & Cappé - COLT, 2011].'''

__author__ = "Olivier Cappé, Aurélien Garivier"
__version__ = "$Revision: 1.11 $"


from kullback import klucbGauss
from policy.klUCB import klUCB

class UCB(klUCB):
    """The Upper Confidence Bound (UCB) index.
      """
    def __init__(self, nbArms, amplitude=1., lower=0.):
        klUCB.__init__(self, nbArms, amplitude, lower, lambda x, d, sig2: klucbGauss(x, d, .25))
