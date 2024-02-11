from dataclasses import dataclass
import itertools
import pprint

gains = [0.5, 1, 2, 4]
dgs = [1, 2, 4]
its = [1, 2, 4, 8]  # *100ms
senss = [1, 1/3]

@dataclass
class config:
    gain: float
    dg: float
    it: float
    sens: float

    def __post_init__(self):
        self.value = self.gain * self.dg * self.it * self.sens
    
    def better_than(self, other):
        assert isinstance(other, config) and self.value == other.value

        it_diff = self.it - other.it
        if it_diff != 0: return it_diff < 0
        
        sens_diff = self.sens - other.sens
        if sens_diff != 0: return sens_diff > 0

        dg_diff = self.dg - other.dg
        if dg_diff != 0: return dg_diff < 0

        gain_diff = self.gain - other.gain
        if gain_diff != 0: return gain_diff < 0

        assert False
    
    def to_ctx(self):
        dg = dgs.index(self.dg)
        gain = gains.index(self.gain)
        sens = senss.index(self.sens)
        it = its.index(self.it)
        return (dg << 5) | (gain << 3) | (sens << 2) | it

best = {}

for var in itertools.product(gains, dgs, its, senss):  # iterate over all possible combinations
    conf = config(*var)
    if conf.sens != 1 and conf.dg != 1: continue

    if not conf.value in best:
        best[conf.value] = conf
    else:
        other = best[conf.value]
        if conf.better_than(other):
            best[conf.value] = conf

pprinter = pprint.PrettyPrinter()
pprinter.pprint(best)
print(f"len(best) = {len(best)}")

min_best = min(best.keys())
best_norm = {}
for k,v in best.items():
    best_norm[k*1/min_best] = v


initializer = [f"{{{int(k)}, {hex(v.to_ctx())}}}" for k,v in sorted(best_norm.items())]
initializer_list = ", ".join(initializer)
print(f"const CONF confs[{len(initializer)}] = {{{initializer_list}}};")
