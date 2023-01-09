        pyBandits: python package for multi-armed bandits
    
      authors: Olivier Cappé, Aurélien Garivier, Emilie Kaufmann
     ............................................................


These files have been tested under python2.6. The C extension requires a C
compiler and installed python header files; it is not required to run the
code: it just speeds up some critical computations (and is recommended for
running policy/KLempUCB).


Files included in the current release:

    demo.py			  Demo script (use this first)		
    Evaluation.py		  Class for running a bandit experiment
    kullback.py			  Module with Kullback-Leibler utilities
    Result.py			  Class for summarizing a bandit experiments
    
    arm/
      arm/__init__.py
      arm/Arm.py		  Generic arm class
      arm/Bernoulli.py		  Class for common arm distributions (possibly
      arm/Exponential.py	    truncated)
      arm/Gaussian.py
      arm/Poisson.py
    
    C/
      C/kullback.c		  C-coded version of the Kullback-Leibler
      C/Makefile		    utility module (will supersede kullback.py
      C/README.txt		    if installed)
      C/setup.py
    
    environment/
      environment/__init__.py
      environment/Environment.py  Generic environment class
      environment/MAB.py	  Multi-armed bandit class (note that a MAB is
    				    a collection of arms and can thus use
    				    arms with differents types)
    
    policy/
      policy/__init__.py
      policy/Policy.py		  Generic policy classes
      policy/IndexPolicy.py
      policy/BayesUCB.py	  Class for policies, names should be explicit
      policy/KLempUCB.py	    (note that klUCB can use the different
      policy/klUCB.py		    forms of KL divergences defined in
      policy/Thompson.py	    kullback, in particular, UCB is a special
      policy/UCB.py                 case of klUCB)
      policy/UCBV.py
    
    posterior/
      posterior/__init__.py
      posterior/Posterior.py	  Generic class for posteriors
      posterior/Beta.py		  Posteriors in Bernoulli/Beta experiments


# HOWTO:

To compile the C functions (which is optional), use "cd C; make".

To run the demo, simply type: "python demo.py". By editing the file, you will
be able to run alternative demos easily, and the way to run other experiments
should be quite straightforward.


# NOTES:

By default, most policies (policy/UCB, policy/KLempUCB, policy/klUCB when used
with the default choice for parameter klucb) require the rewards to be bounded
in [0,1], but other bounds can be used thanks to the parameters 'amplitude'
and 'lower': the rewards must then be no smaller than 'lower' and no larger
than 'lower+amplitude'.

Warning: arguments 'lower' and 'amplitude' should not be modified when
policy/klUCB is used with a distribution-specific divergence 'klucb', even for
bounded rewards. For instance, when policy/klUCB(klucb=klucbPoisson) is used
with 'arm/Poisson(2, 10)', the parameter 'amplitude' should not be set to 10
in policy/klUCB(klucb=klucbPoisson) because that would cause the rewards to be
inadequately divided by 10, see the scenario 1 in 'demo.py'.


    --
    $Id: README.txt,v 1.7 2012-07-05 17:03:40 cappe Exp $
