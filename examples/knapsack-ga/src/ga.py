import random
import qrng
import numpy as np
import random as rd
from random import randint
import matplotlib.pyplot as plt


class Ga:
    def __init__(self,
                 chromosome_size,
                 gene_low,
                 gene_high,
                 population_size,
                 mutation_rate,
                 crossover_rate,
                 fitness_func,# pointer to fitness function
                 number_of_generations,
                 gene_type="int",
                 crossover_type="single-point",
                 tournament_size=3,
                 number_parents_mating=2,
                 selection_type="random-selection",
                 save_best_solutions=False,
                 save_solutions=False,
                 allow_duplicate_gene=True,
                 stop_criteria_saturate=-1,
                 stop_fitness_target_value = -1,
                 on_generation_cbk=None, # pointer to callback functions
                 on_population_init_cbk=None # pointer to callback functions

                 ):

        self.__chromosome_size = chromosome_size
        self.__gene_low = gene_low
        self.__gene_high = gene_high
        self.__population_size = population_size
        self.__mutation_rate = mutation_rate
        self.__crossover_rate = crossover_rate
        self.__fitness_func = fitness_func
        self.__number_of_generations = number_of_generations
        self.__gene_type = gene_type,
        self.__crossover_type = crossover_type
        self.__selection_type = selection_type
        self.__tournament_size = tournament_size
        self.__number_parents_mating = number_parents_mating
        self.__save_best_solutions = save_best_solutions
        self.__save_solutions = save_solutions
        self.__stop_criteria_saturate = stop_criteria_saturate
        self.__allow_duplicate_gene = allow_duplicate_gene
        self.__stop_fitness_target_value = stop_fitness_target_value
        self.__on_generation_cbk = on_generation_cbk
        self.__on_population_init_cbk = on_population_init_cbk
        # set crossover function
        self.__crossover_function = self.__crossover_single_point
        if self.__crossover_type == "single-point":
            self.__crossover_function = self.__crossover_single_point
        elif self.__crossover_type == "2-point":
            self.__crossover_function = self.__crossover_two_points
        elif self.__crossover_type == "uniform":
            self.__crossover_function = self.__crossover_uniform
        elif self.__crossover_type == "scattered":
            self.__crossover_function = self.__crossover_scattered
        else:
            raise TypeError("Crossover function does not exist.")

        # set selection_type
        self.__selection_function = self.__select_random
        if self.__selection_type == "steady-state-selection":
            self.__selection_function = self.__select_steady_state
        elif self.__selection_type == "rank-selection":
            self.__selection_function = self.__select_rank
        elif self.__selection_type == "random-selection":
            self.__selection_function = self.__select_random
        elif self.__selection_type == "tournament-selection":
            self.__selection_function = self.__select_tournament
        elif self.__selection_type == "roulette-wheel-selection":
            self.__selection_function = self.__select_roulette_wheel
        elif self.__selection_type == "stochastic-universal-selection":
            self.__selection_function = self.__select_stochastic_universal
        else:
            raise TypeError("Selection function does not exist")

        # generic variables
        self.current_generation = 0
        self.__fitness_history = []
        self.solutions = []
        self.best_solutions = []
        self.best_solution = None
        self.best_fitness=0
        self.__old_fitness_value = 0
        self.__saturation_counter = 0

    def __evaluate(self, population):
        calculated_fitness = np.empty(self.__population_size)
        for i in range(self.__population_size):
            calculated_fitness[i] = self.__fitness_func(i, population[i])
        return calculated_fitness.astype(int)

    def __select_steady_state(self, fitness_values, num_parents, population):
        fitness_sorted = sorted(range(len(fitness_values)), key=lambda e: fitness_values[e])
        fitness_sorted.reverse()

        parents = np.empty((num_parents, self.__chromosome_size))
        for i in range(num_parents):
            parents[i, :] = population[fitness_sorted[i], :].copy()
        return parents

    def __select_rank(self, fitness_values, num_parents, population):
        fitness_sorted = sorted(range(len(fitness_values)), key=lambda e: fitness_values[e])
        fitness_sorted.reverse()

        parents = np.empty((num_parents, self.__chromosome_size))
        for i in range(num_parents):
            parents[i, :] = population[fitness_sorted[i], :].copy()
        return parents

    def __select_random(self, fitness_values, num_parents, population):
        parents = np.empty((num_parents, self.__chromosome_size))
        rand_indices = np.random.randint(low=0.0, high=fitness_values.shape[0], size=num_parents)

        for i in range(num_parents):
            parents[i, :] = population[rand_indices[i], :].copy()
        return parents

    def __select_tournament(self, fitness_values, num_parents, population):
        parents = np.empty((num_parents, self.__chromosome_size))
        for i in range(num_parents):
            rand_indices = np.random.randint(low=0.0, high=fitness_values.shape[0], size=self.__tournament_size)
            k_fitness = fitness_values[rand_indices]
            selected_parent_idx = np.where(k_fitness == np.max(k_fitness))[0][0]

            parents[i, :] = population[rand_indices[selected_parent_idx], :].copy()
        return parents

    def __select_roulette_wheel(self, fitness_values, num_parents, population):
        fitness_sum = np.sum(fitness_values)
        if fitness_sum != 0:
            probs = fitness_values / fitness_sum
        else:
            probs = fitness_values / np.finfo(np.float64).max
        probs_start = np.zeros(probs.shape, dtype=float)
        probs_end = np.zeros(probs.shape, dtype=float)

        current = 0.0

        for _ in range(probs.shape[0]):
            min_probs_idx = np.where(probs == np.min(probs))[0][0]
            probs_start[min_probs_idx] = current
            current = current + probs[min_probs_idx]
            probs_end[min_probs_idx] = current
            probs[min_probs_idx] = np.iinfo(np.int64).max

        parents = np.empty((num_parents, self.__chromosome_size))
        for i in range(num_parents):
            rand_prob = qrng.qrand_rand_float(0.0,1.0,1)
            for idx in range(probs.shape[0]):
                if probs_start[idx] <= rand_prob < probs_end[idx]:
                    parents[i, :] = population[idx, :].copy()
                    break
        return parents

    def __select_stochastic_universal(self, fitness_values, num_parents, population):
        fitness_sum = np.sum(fitness_values)
        if fitness_sum != 0:
            probs = fitness_values / fitness_sum
        else:
            probs = fitness_values / np.finfo(np.float64).eps
        probs_start = np.zeros(probs.shape, dtype=float)
        probs_end = np.zeros(probs.shape, dtype=float)

        current = 0.0

        for _ in range(probs.shape[0]):
            min_probs_idx = np.where(probs == np.min(probs))[0][0]
            probs_start[min_probs_idx] = current
            current = current + probs[min_probs_idx]
            probs_end[min_probs_idx] = current
            probs[min_probs_idx] = np.iinfo(np.int64).max

        pointers_distance = 1.0 / self.__number_parents_mating
        first_pointer = np.random.uniform(low=0.0, high=pointers_distance, size=1)

        parents = np.empty((num_parents, self.__chromosome_size))

        for i in range(num_parents):
            rand_pointer = first_pointer + i * pointers_distance
            for idx in range(probs.shape[0]):
                if probs_start[idx] <= rand_pointer < probs_end[idx]:
                    parents[i, :] = population[idx, :].copy()
                    break
        return parents

    def __crossover_single_point(self, parents, num_offsprings):
        offsprings = np.empty((num_offsprings, self.__chromosome_size))
        for k in range(num_offsprings):
            crossover_point = np.random.randint(low=0, high=self.__chromosome_size, size=1)[0]
            if not (self.__crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= self.__crossover_rate)[0]

                if len(indices) == 0:
                    offsprings[k, :] = parents[k % parents.shape[0], :]
                    continue
                elif len(indices) == 1:
                    parent1_idx = indices[0]
                    parent2_idx = parent1_idx
                else:
                    indices = random.sample(set(indices), 2)
                    parent1_idx = indices[0]
                    parent2_idx = indices[1]
            else:
                parent1_idx = k % parents.shape[0]
                parent2_idx = (k + 1) % parents.shape[0]

            offsprings[k, 0:crossover_point] = parents[parent1_idx, 0:crossover_point]
            offsprings[k, crossover_point:] = parents[parent2_idx, crossover_point:]
            # TODO: solve duplicated genes
        return offsprings

    def __crossover_two_points(self, parents, num_offsprings):
        offsprings = np.empty((num_offsprings, self.__chromosome_size))

        for k in range(num_offsprings):
            if parents.shape[1] == 1:
                crossover_point1 = 0
            else:
                crossover_point1 = np.random.randint(low=0, high=np.ceil(self.__chromosome_size / 2 + 1), size=1)[0]

            crossover_point2 = crossover_point1 + int(self.__chromosome_size / 2)

            if not (self.__crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= self.__crossover_rate)[0]

                if len(indices) == 0:
                    offsprings[k, :] = parents[k % parents.shape[0], :]
                    continue
                elif len(indices) == 1:
                    parent1_idx = indices[0]
                    parent2_idx = parent1_idx
                else:
                    indices = np.random.choice(indices, 2)
                    parent1_idx = indices[0]
                    parent2_idx = indices[1]
            else:
                parent1_idx = k % parents.shape[0]
                parent2_idx = (k + 1) % parents.shape[0]

            offsprings[k, 0:crossover_point1] = parents[parent1_idx, 0:crossover_point1]

            offsprings[k, crossover_point2:] = parents[parent1_idx, crossover_point2:]

            offsprings[k, crossover_point1:crossover_point2] = parents[parent2_idx, crossover_point1:crossover_point2]
            # TODO: solve duplicated genes
        return offsprings

    def __crossover_uniform(self, parents, num_offsprings):
        offsprings = np.empty((num_offsprings, self.__chromosome_size))
        for k in range(num_offsprings):
            if not (self.__crossover_rate is None):

                #probs = np.random.random(size=parents.shape[0])
                probs = np.array(qrng.qrand_rand_float(0,1,parents.shape[0]))
                
                indices = np.where(probs <= self.__crossover_rate)[0]

                if len(indices) == 0:
                    offsprings[k, :] = parents[k % parents.shape[0], :]
                    continue
                elif len(indices) == 1:
                    parent1_idx = indices[0]
                    parent2_idx = parent1_idx
                else:
                    indices = np.random.choice(indices, 2)
                    parent1_idx = indices[0]
                    parent2_idx = indices[1]
            else:
                parent1_idx = k % parents.shape[0]
                parent2_idx = (k + 1) % parents.shape[0]

            #genes_source = np.random.randint(low=0, high=2, size=self.__chromosome_size)
            genes_source = np.array(qrng.qrand_rand_int(0, 2, self.__chromosome_size))
            for gene_idx in range(self.__chromosome_size):
                if genes_source[gene_idx] == 0:
                    offsprings[k, gene_idx] = parents[parent1_idx, gene_idx]
                elif genes_source[gene_idx] == 1:
                    offsprings[k, gene_idx] = parents[parent2_idx, gene_idx]
        return offsprings

    def __crossover_scattered(self, parents, num_offsprings):
        offsprings = np.empty((num_offsprings, self.__chromosome_size))

        for k in range(num_offsprings):
            if not (self.__crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= self.__crossover_rate)[0]

                if len(indices) == 0:
                    offsprings[k, :] = parents[k % parents.shape[0], :]
                    continue
                elif len(indices) == 1:
                    parent1_idx = indices[0]
                    parent2_idx = parent1_idx
                else:
                    indices = np.random.choice(indices, 2)
                    parent1_idx = indices[0]
                    parent2_idx = indices[1]
            else:
                # Index of the first parent to mate.
                parent1_idx = k % parents.shape[0]
                # Index of the second parent to mate.
                parent2_idx = (k + 1) % parents.shape[0]

            gene_sources = np.random.randint(0, 2, size=self.__chromosome_size)
            offsprings[k, :] = np.where(gene_sources == 0, parents[parent1_idx, :], parents[parent2_idx, :])
        return offsprings

    def __mutate(self, offsprings):
        mutants = np.empty(offsprings.shape)
        for i in range(mutants.shape[0]):
            #random_value = rd.random()
            random_value = qrng.qrand_rand_int(0,1,1)[0]
            mutants[i, :] = offsprings[i, :]
            if random_value > self.__mutation_rate:
                continue
            #int_random_value = randint(0, offsprings.shape[1] - 1)
            int_random_value = qrng.qrand_rand_int(0, offsprings.shape[1] - 1,1)[0]
            if mutants[i, int_random_value] == 0:
                mutants[i, int_random_value] = 1
            else:
                mutants[i, int_random_value] = 0
        return mutants

    def run(self):
        qrng.qrand_init("random.cs.upt.ro")
        num_parents = int(self.__population_size / 2)
        num_offsprings = self.__population_size - num_parents
        # create population
        if self.__on_population_init_cbk is None:
            # population = np.random.randint(low=self.__gene_low,
            #                            high=self.__gene_high,
            #                            size=(self.__population_size, self.__chromosome_size))
            population = np.zeros((self.__population_size, self.__chromosome_size))
            for item in range(0, self.__population_size):
                population[item] = qrng.qrand_rand_int(self.__gene_low, self.__gene_high,self.__chromosome_size)
        else:
            population = self.__on_population_init_cbk(self.__gene_low,self.__gene_high,self.__population_size,self.__chromosome_size)

        for i in range(self.__number_of_generations):
            self.current_generation = i
            current_fitness = self.__evaluate(population)
            best_fitness_index = np.argmax(current_fitness)
            self.best_solution = population[best_fitness_index, :].copy()
            self.best_fitness = current_fitness[best_fitness_index]
            self.__fitness_history.append(current_fitness)

            if self.__save_best_solutions:
                self.best_solutions.append(self.best_solution)
            if self.__save_solutions:
                self.solutions.append(population)
            if self.__stop_criteria_saturate != -1:
                if self.__old_fitness_value == self.best_fitness:
                    self.__saturation_counter+=1
                else:
                    self.__old_fitness_value = self.best_fitness
                    self.__saturation_counter=0
                if self.__saturation_counter == self.__stop_criteria_saturate:
                    self.__number_of_generations = i+1
                    break;
            if self.__stop_fitness_target_value != -1:
                if self.__stop_fitness_target_value == self.best_fitness:
                    self.__number_of_generations = i + 1
                    break;

            parents = self.__selection_function(current_fitness, num_parents, population)
            offsprings = self.__crossover_function(parents, num_offsprings)
            mutants = self.__mutate(offsprings)
            population[0:parents.shape[0], :] = parents
            population[parents.shape[0]:, :] = mutants
            if self.__on_generation_cbk is not None:
                ret_value = self.__on_generation_cbk(self)
                if ret_value != 0:
                    self.__number_of_generations = i + 1
                    break

        fitness_last_gen = self.__evaluate(population)
        best_fitness_index = np.argmax(fitness_last_gen)
        self.best_solution = population[best_fitness_index, :].copy()
        self.best_fitness = fitness_last_gen[best_fitness_index]

        #print('Last generation: \n{}\n'.format(population))
        qrng.qrand_close()
        return self.best_solution, self.best_fitness

    def visualize(self):
        fitness_history_mean = [np.mean(fitness_value) for fitness_value in self.__fitness_history]
        fitness_history_max = [np.max(fitness_value) for fitness_value in self.__fitness_history]
        plt.plot(list(range(self.__number_of_generations)), fitness_history_mean, label='Mean Fitness')
        plt.plot(list(range(self.__number_of_generations)), fitness_history_max, label='Max Fitness')
        plt.legend()
        plt.title('Fitness through the generations')
        plt.xlabel('Generations')
        plt.ylabel('Fitness')
        plt.savefig("Figure_2.png")
        plt.show()


