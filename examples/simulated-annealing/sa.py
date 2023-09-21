import numpy as np
import matplotlib.pyplot as plt
from util.pyqrng import randint, uniform

class Sa:
    def __init__(self,
                 problem_domain,
                 on_cost_hdl,
                 max_steps,
                 generate_initial_state_hdl=None,
                 on_step_completed_cbk=None,
                 ):

        self.problem_domain = problem_domain
        self.__on_cost_hdl = on_cost_hdl
        self.__max_steps = max_steps

        if generate_initial_state_hdl is None:
            self.__state_generation = self.__initialize
        else:
            self.__state_generation = generate_initial_state_hdl
        self.__on_step_completed_cbk = on_step_completed_cbk

        self.state = self.__state_generation(self)

        self.cost = self.__on_cost_hdl(self.state)
        self.states, self.costs = [self.state], [self.cost]
        self.actual_steps = 0
    def __neighbor_selection(self, state, fraction):
        amplitude = (max(self.problem_domain) - min(self.problem_domain)) * fraction / 10
        delta = (-amplitude / 2.) + amplitude * uniform(0,1,1)[0]

        def clip(x):
            """ Force x to be in the interval."""
            a, b = self.problem_domain
            return max(min(x, b), a)

        return clip(state + delta)

    def __get_acceptance_probability(self, cost, new_cost, temperature):
        if new_cost < cost:
            return 1
        else:
            p = np.exp(- (new_cost - cost) / temperature)
            return p

    def __initialize(self, instance):
        a, b = self.problem_domain
        return a + (b - a) * uniform(0,1,1)[0]

    def run(self):

        for step in range(self.__max_steps):
            fraction = step / float(self.__max_steps)
            T = max(0.01, min(1, 1 - fraction))
            new_state = self.__neighbor_selection(self.state, fraction)
            new_cost = self.__on_cost_hdl(new_state)

            if self.__get_acceptance_probability(self.cost, new_cost, T) > uniform(0,1,1)[0]:
                self.state, self.cost = new_state, new_cost
                self.states.append(self.state)
                self.costs.append(self.cost)
                self.actual_steps += 1

            if self.__on_step_completed_cbk is not None:
                self.__on_step_completed_cbk(self)

        return self.state, self.cost

    def visualize(self):
        plt.figure()
        plt.suptitle("Evolution of states and costs of the simulated annealing")
        plt.subplot(121)
        plt.plot([i for i in range(0, self.actual_steps+1)], self.states, 'r')
        plt.xlabel('Steps')
        plt.ylabel('States')
        plt.subplot(122)
        plt.plot([i for i in range(0, self.actual_steps+1)], self.costs, 'b')
        plt.xlabel('Steps')
        plt.ylabel('Costs')
        plt.savefig("Figure_1.png")

    def save_to_file(self,file):
        number_of_steps = self.actual_steps+1
        states_array = self.states
        costs_array = self.costs
        with open(file, 'w') as f:
            f.write(f'Number of steps: {number_of_steps}')
            f.write('\n')
            f.write(f'States array: {states_array}')
            f.write('\n')
            f.write(f'Costs array: {costs_array}')


def f(x):
    """ Function to minimize."""
    return x**2

def generate_initial_state(sa_instance):
    a, b = sa_instance.problem_domain
    return a + (b - a) * uniform(0,1,1)[0]

def cost_function(x):
    return f(x)

if __name__ == '__main__':
    interval = (-10, 10)
    sa_instance = Sa(
                 #generate_initial_state_hndl = generate_initial_state,
                 problem_domain=interval,
                 on_cost_hdl=cost_function,
                 max_steps=200
                 )
    state, cost = sa_instance.run()
    print("State: {0} Cost {1}".format(state,cost))
    sa_instance.visualize()
    sa_instance.save_to_file('quantum_random_sa')
