# -*- coding: utf-8 -*-
'''The Thompson (Bayesian) index policy.
Reference: [Thompson - Biometrika, 1933].
'''

__author__ = "Olivier Cappé, Aurélien Garivier, Emilie Kaufmann"
__version__ = "$Revision: 1.9 $"


from random import choice

from policy.IndexPolicy import IndexPolicy

class Thompson(IndexPolicy):
  """The Thompson (Bayesian) index policy.
  Reference: [Thompson - Biometrika, 1933].
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
    return self.posterior[arm].sample()
