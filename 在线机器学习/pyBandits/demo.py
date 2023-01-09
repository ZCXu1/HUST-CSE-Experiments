# -*- coding: utf-8 -*-
'''Demonstration file for the pyBandits package'''

__author__ = "Olivier Cappé, Aurélien Garivier, Emilie Kaufmann"
__version__ = "$Revision: 1.6 $"


from environment.MAB import MAB
from arm.Bernoulli import Bernoulli
from arm.Poisson import Poisson
from arm.Exponential import Exponential
from arm.MyArm import MyArm
from policy.UCB import UCB
from numpy import *
from matplotlib.pyplot import *

from policy.UCBV import UCBV
from policy.klUCB import klUCB
from policy.KLempUCB import KLempUCB
from policy.Thompson import Thompson
from policy.BayesUCB import BayesUCB
from Evaluation import *
from kullback import *
from posterior.Beta import Beta



colors = ['blue', 'green', 'red', 'cyan', 'magenta', 'black']
graphic = 'yes'
scenario = 0
nbRep = 10
horizon = 2000

if scenario == 0:
    # First scenario (default): Bernoulli experiment with ten arms
    # (figure 2 in [Garivier & Cappé, COLT 2011])
    env = MAB([MyArm(p) for p in range(0,9)])
    print(env.nbArms)
    policies = [UCB(env.nbArms), klUCB(env.nbArms), BayesUCB(env.nbArms,Beta), Thompson(env.nbArms,Beta)]
elif scenario == 1:
    # Second scenario: Truncated Poissson distrubtions
    trunc = 10
    env = MAB([Poisson(0.5+0.25*a, trunc) for a in range(1,7)])
    K = env.nbArms
    policies = [UCB(K, trunc), UCBV(K, trunc), klUCB(K, trunc), klUCB(K, klucb=klucbPoisson),KLempUCB(K, trunc)]
else:
    # Third scenario: Truncated exponential distributions
    trunc = 10
    env = MAB([Exponential(1./p, trunc) for p in range(1, 6)])
    K = env.nbArms
    policies = [UCB(K, trunc), UCBV(K, trunc), klUCB(K, trunc), klUCB(K, klucb=klucbExp), KLempUCB(K, trunc)]

tsav = int_(linspace(100,horizon-1,200))

if graphic == 'yes':
    figure(1)

k=0


for policy in policies:
    ev = Evaluation(env, policy, nbRep, horizon, tsav)
    print("meanReward:".format(),ev.meanReward())
    print ("meanNbDraws:".format(),ev.meanNbDraws())
    meanRegret = (ev.meanRegret())
    print("meanRegret: ".format(),meanRegret.shape)
    if graphic == 'yes':
        semilogx(1+tsav, meanRegret, color = colors[k])
        xlabel('Time')
        ylabel('Regret')
    k = k+1

if graphic == 'yes':
    legend([policy.__class__.__name__ for policy in policies], loc=0)
    title('Average regret for various policies')
    show()
