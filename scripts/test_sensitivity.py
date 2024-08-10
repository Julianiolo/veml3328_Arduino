from sensitivity import best_norm

auto_steps = sorted(best_norm.keys())
auto_steps = [int(x) for x in auto_steps]

threshold_value = int(0xffff*0.85)

max_sens = max(auto_steps)
max_val = max_sens*0xffff

test_sens = auto_steps[1]

def get_sensed(universal_value, sensitivity):
    return int(universal_value * (sensitivity/max_sens))

def get_universal(sensed, sensitivity):
    return int(sensed * (max_sens/sensitivity))

def get_sens(value):
    if value > threshold_value:
        return auto_steps[0]; # if already basically maxed out when using one above minimum sensitivity, then we use minimum sensitivity

    # if value < 0xffff:
    #     return auto_steps[15]; # if no light is sensed, we can just go staight to maximum sensitivity, as the light must be so dim, that 768/2 times the value still can't overflow

    universal_value = get_universal(value, test_sens)

    i = 0
    while i+1 < 16:
        v_s = int(universal_value * auto_steps[i+1]/max_sens) # simulated value for next sensitivity step
        if v_s > threshold_value: # check if value would overflow for next sensitivity step
            break
        i += 1

    return auto_steps[i]



for actual in range(0, max_val):
    sensed = get_sensed(actual, test_sens)
    x = get_universal(sensed, test_sens)
    best_sens = get_sens(sensed)
    then_sensed = get_sensed(actual, best_sens)

    if then_sensed > threshold_value*1.1 and best_sens != 1:
        a = 0
    
    if best_sens != max_sens:
        next_step_sensed = get_sensed(actual, auto_steps[auto_steps.index(best_sens)+1])
        if next_step_sensed < threshold_value:
            a = 0

print("done :)")
