import random
from .pyqrng import randint, uniform
import numpy as np
import matplotlib.pyplot as plt

fitness_history = []

def run(chromosome_size, 
    gene_low, 
    gene_high, 
    population_size, 
    mutation_rate,
    crossover_rate,
    fitness_func,
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
    ):

    current_generation = 0
    
    solutions = []
    best_solutions = []
    best_solution = np.zeros(chromosome_size, dtype=np.uint32)
    best_fitness=0
    old_fitness_value = 0
    saturation_counter = 0
    num_parents = int(population_size / 2)
    num_offsprings = population_size - num_parents
    

    def __select_steady_state(fitness_values, num_parents, population, chromosome_size, number_parents_mating, tournament_size):
        fitness_sorted = sorted(range(len(fitness_values)), key=lambda e: fitness_values[e])
        fitness_sorted.reverse()

        parents = np.zeros((num_parents, chromosome_size), dtype=np.uint32)
        for i in range(num_parents):
            parents[i, :] = population[fitness_sorted[i], :].copy()
        return parents

    def __select_rank(fitness_values, num_parents, population, chromosome_size, number_parents_mating, tournament_size):
        fitness_sorted = sorted(range(len(fitness_values)), key=lambda e: fitness_values[e])
        fitness_sorted.reverse()

        parents = np.zeros((num_parents, chromosome_size), dtype=np.uint32)
        for i in range(num_parents):
            parents[i, :] = population[fitness_sorted[i], :].copy()
        return parents

    def __select_random(fitness_values, num_parents, population, chromosome_size, number_parents_mating, tournament_size):
        parents = np.zeros((num_parents, chromosome_size), dtype=np.uint32)
        rand_indices = np.random.randint(low=0.0, high=fitness_values.shape[0], size=num_parents)

        for i in range(num_parents):
            parents[i, :] = population[rand_indices[i], :].copy()
        return parents

    def __select_tournament(fitness_values, num_parents, population, chromosome_size, number_parents_mating, tournament_size):
        parents = np.empty((num_parents, chromosome_size), dtype = np.uint32)
        for i in range(num_parents):
            rand_indices = np.random.randint(low=0.0, high=fitness_values.shape[0], size=tournament_size)
            k_fitness = fitness_values[rand_indices]
            selected_parent_idx = np.where(k_fitness == np.max(k_fitness))[0][0]

            parents[i, :] = population[rand_indices[selected_parent_idx], :].copy()
        return parents


    def __select_stochastic_universal(fitness_values, num_parents, population, chromosome_size, number_parents_mating):
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

        pointers_distance = 1.0 / number_parents_mating
        first_pointer = np.random.uniform(low=0.0, high=pointers_distance, size=1)

        parents = np.zeros((num_parents, chromosome_size), dtype=np.uint32)

        for i in range(num_parents):
            rand_pointer = first_pointer + i * pointers_distance
            for idx in range(probs.shape[0]):
                if probs_start[idx] <= rand_pointer < probs_end[idx]:
                    parents[i, :] = population[idx, :].copy()
                    break
        return parents

    def __select_roulette_wheel(fitness_values, num_parents, population, chromosome_size, number_parents_mating, tournament_size):
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

        parents = np.zeros((num_parents, chromosome_size), dtype=np.uint32)
        for i in range(num_parents):
            rand_prob = uniform(0.0,1.0,1)[0]
            for idx in range(probs.shape[0]):
                if probs_start[idx] <= rand_prob < probs_end[idx]:
                    parents[i, :] = population[idx, :].copy()
                    break
        return parents

    def __crossover_uniform(parents, num_offsprings, chromosome_size, crossover_rate):
        offsprings = np.zeros((num_offsprings, chromosome_size), dtype=np.uint32)
        for k in range(num_offsprings):
            if not (crossover_rate is None):
                probs = uniform(0,1,parents.shape[0]).copy()
                indices = np.where(probs <= crossover_rate)[0]

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


            genes_source = randint(0,2,chromosome_size).copy()
            for gene_idx in range(chromosome_size):
                if genes_source[gene_idx] == 0:
                    offsprings[k, gene_idx] = parents[parent1_idx, gene_idx]
                elif genes_source[gene_idx] == 1:
                    offsprings[k, gene_idx] = parents[parent2_idx, gene_idx]
        return offsprings

    def __crossover_single_point( parents, num_offsprings, chromosome_size, crossover_rate):
        offsprings = np.zeros((num_offsprings, chromosome_size), dtype = np.uint32)
        for k in range(num_offsprings):
            crossover_point = np.random.randint(low=0, high=chromosome_size, size=1)[0]
            if not (crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= crossover_rate)[0]

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
        return offsprings

    def __crossover_two_points( parents, num_offsprings, chromosome_size, crossover_rate):
        offsprings = np.zeros((num_offsprings, chromosome_size), dtype=np.uint32)

        for k in range(num_offsprings):
            if parents.shape[1] == 1:
                crossover_point1 = 0
            else:
                crossover_point1 = np.random.randint(low=0, high=np.ceil(chromosome_size / 2 + 1), size=1)[0]

            crossover_point2 = crossover_point1 + int(chromosome_size / 2)

            if not (crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= crossover_rate)[0]

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

        return offsprings

       

    def __crossover_scattered( parents, num_offsprings, chromosome_size, crossover_rate):
        offsprings = np.zeros((num_offsprings, chromosome_size), dtype=np.uint32)

        for k in range(num_offsprings):
            if not (crossover_rate is None):
                probs = np.random.random(size=parents.shape[0])
                indices = np.where(probs <= crossover_rate)[0]

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

            gene_sources = np.random.randint(0, 2, size=chromosome_size)
            offsprings[k, :] = np.where(gene_sources == 0, parents[parent1_idx, :], parents[parent2_idx, :])
        return offsprings

    def __evaluate(population, population_size, fitness_func):
        calculated_fitness = np.zeros(population_size,dtype=float)
        for i in range(population_size):
            calculated_fitness[i] = fitness_func(i, population[i])
        return calculated_fitness

    def __mutate(offsprings, mutation_rate):
        mutants = np.zeros(offsprings.shape, dtype=np.uint32)
        for i in range(mutants.shape[0]):
            random_value = uniform(0.0,1.0,1)[0]
            mutants[i, :] = offsprings[i, :]
            if random_value > mutation_rate:
                continue
            #int_random_value = randint(0, offsprings.shape[1] - 1)
    #            int_random_value = qrand_rand_int(0, offsprings.shape[1] - 1,1)[0]
            int_random_value = randint(0, offsprings.shape[1] - 1, 1)[0]
            if mutants[i, int_random_value] == 0:
                mutants[i, int_random_value] = 1
            else:
                mutants[i, int_random_value] = 0
        return mutants

    crossover_function = __crossover_single_point
    if crossover_type == "single-point":
        crossover_function = __crossover_single_point
    elif crossover_type == "2-point":
        crossover_function = __crossover_two_points
    elif crossover_type == "uniform":
        crossover_function = __crossover_uniform
    elif crossover_type == "scattered":
        crossover_function = __crossover_scattered
    else:
        raise TypeError("Crossover function does not exist.")

    selection_function = __select_random
    if selection_type == "steady-state-selection":
        selection_function = __select_steady_state
    elif selection_type == "rank-selection":
        selection_function = __select_rank
    elif selection_type == "random-selection":
        selection_function = __select_random
    elif selection_type == "tournament-selection":
        selection_function = __select_tournament
    elif selection_type == "roulette-wheel-selection":
        selection_function = __select_roulette_wheel
    elif selection_type == "stochastic-universal-selection":
        selection_function = __select_stochastic_universal
    else:
        raise TypeError("Selection function does not exist")
    # create population
    
    population = np.zeros((population_size, chromosome_size),dtype=np.uint32)
    for item in range(0, population_size):
        population[item] = randint(gene_low, gene_high,chromosome_size).copy()
    
    for i in range(number_of_generations):
        current_generation = i
        current_fitness = __evaluate(population, population_size,fitness_func)
        best_fitness_index = np.argmax(current_fitness)
        best_solution = population[best_fitness_index, :].copy()
        best_fitness = current_fitness[best_fitness_index]
        fitness_history.append(current_fitness)

        if save_best_solutions:
            best_solutions.append(best_solution)
        if save_solutions:
            solutions.append(population)
        if stop_criteria_saturate != -1:
            if old_fitness_value == best_fitness:
                saturation_counter+=1
            else:
                old_fitness_value = best_fitness
                saturation_counter=0
            if saturation_counter == stop_criteria_saturate:
                number_of_generations = i+1
                break;
        if stop_fitness_target_value != -1:
            if stop_fitness_target_value == best_fitness:
                number_of_generations = i + 1
                break;

        parents = selection_function(current_fitness, num_parents, population, chromosome_size, number_parents_mating, tournament_size)
        offsprings = crossover_function(parents, num_offsprings, chromosome_size, crossover_rate)
        mutants = __mutate(offsprings, mutation_rate)
        population[0:parents.shape[0], :] = parents
        population[parents.shape[0]:, :] = mutants
        if on_generation_cbk is not None:
            ret_value = on_generation_cbk(current_generation, best_solution, best_fitness)
            if ret_value != 0:
                number_of_generations = i + 1
                break

    fitness_last_gen = __evaluate(population, population_size,fitness_func)
    best_fitness_index = np.argmax(fitness_last_gen)
    best_solution = population[best_fitness_index, :].copy()
    best_fitness = fitness_last_gen[best_fitness_index]

    #print('Last generation: \n{}\n'.format(population))
    return best_solution, best_fitness


def visualize():
    number_of_generations = len(fitness_history)
    fitness_history_mean = [np.mean(fitness_value) for fitness_value in fitness_history]
    fitness_history_max = [np.max(fitness_value) for fitness_value in fitness_history]
    plt.plot(list(range(number_of_generations)), fitness_history_mean, label='Mean Fitness')
    plt.plot(list(range(number_of_generations)), fitness_history_max, label='Max Fitness')
    plt.legend()
    plt.title('Fitness through the generations')
    plt.xlabel('Generations')
    plt.ylabel('Fitness')
    plt.savefig("Figure_2.png")
    plt.show()
