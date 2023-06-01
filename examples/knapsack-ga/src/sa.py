import numpy as np
import matplotlib.pyplot as plt


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

    def __neighbor_selection(self, state, fraction):
        amplitude = (max(self.problem_domain) - min(self.problem_domain)) * fraction / 10
        delta = (-amplitude / 2.) + amplitude * np.random.random_sample()

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
        return a + (b - a) * np.random.random_sample()

    def run(self):

        for step in range(self.__max_steps):
            fraction = step / float(self.__max_steps)
            T = max(0.01, min(1, 1 - fraction))
            new_state = self.__neighbor_selection(self.state, fraction)
            new_cost = self.__on_cost_hdl(new_state)

            if self.__get_acceptance_probability(self.cost, new_cost, T) > np.random.random():
                self.state, self.cost = new_state, new_cost
                self.states.append(self.state)
                self.costs.append(self.cost)

            if self.__on_step_completed_cbk is not None:
                self.__on_step_completed_cbk(self)

        return self.state, self.cost

    def visualize(self):
        plt.figure()
        plt.suptitle("Evolution of states and costs of the simulated annealing")
        plt.subplot(121)
        plt.plot(self.states, 'r')
        plt.title("States")
        plt.subplot(122)
        plt.plot(self.costs, 'b')
        plt.title("Costs")
        plt.show()

