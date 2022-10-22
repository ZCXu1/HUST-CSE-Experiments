#ifndef _HOOK_H
#define _HOOK_H

unsigned int hook_main(void *priv,struct sk_buff *skb,const struct nf_hook_state *state);

unsigned int hook_nat_in(void *priv,struct sk_buff *skb,const struct nf_hook_state *state);
unsigned int hook_nat_out(void *priv,struct sk_buff *skb,const struct nf_hook_state *state);

#endif