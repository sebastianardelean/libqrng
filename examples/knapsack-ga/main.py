from util.ga import run, visualize,save_to_file

import numpy as np




def to_binary(value, number_of_bits, lsb=False):
    if lsb:
        return np.flip(np.array(list(np.binary_repr(value, number_of_bits)), dtype=int))
    return np.array(list(np.binary_repr(value, number_of_bits)), dtype=int)


def to_decimal(x):
    x = np.flip(x)
    y = 0
    for i, j in enumerate(x):
        y += j << i
    return y


MAX_ALLOWED_MASS = 20
TOTAL_VALUE = 0


#items = [(np.random.randint(1,50),np.random.randint(1,100)) for _ in range(0,50)]
items = [(10, 96), (13, 38), (15, 71), (1, 88), (4, 93), (18, 32), (16, 85), (7, 50), (15, 30), (13, 90), (18, 3), (7, 61), (4, 41), (19, 35), (16, 19), (14, 18), (13, 56), (1, 74), (19, 32), (3, 42), (10, 43), (3, 29), (3, 5), (18, 14), (7, 93), (19, 32), (3, 11), (3, 85), (19, 44), (2, 55), (2, 95), (13, 76)]


def on_generation_callback(current_generation, best_solution,best_fitness):
    print("Generation: ", current_generation)
    print("Best solution {0} Fitness {1}".format(best_solution, best_fitness))
    return 0


def fitness_function(solution_idx, solution):
    total_value = 0
    total_weight = 0
    for i in range(0,len(solution)):
        if solution[i] == 1:
            item = items[i]
            total_value+=item[1]

    for i in range(0, len(solution)):
        if solution[i] == 1:
            item = items[i]
            total_weight+=item[0]

    if total_weight < MAX_ALLOWED_MASS:
        return total_value
    return 0





if __name__ == '__main__':
    solution, fitness = run(
        chromosome_size=len(items),
        gene_low=0,
        gene_high=2,
        population_size=100,
        mutation_rate=0.02,
        crossover_rate=0.8,
        fitness_func=fitness_function,
        number_of_generations=50,
        crossover_type="uniform",
        selection_type="roulette-wheel-selection",
        stop_criteria_saturate=100,
        on_generation_cbk=on_generation_callback,
    )
    
    print(f"Item configuration: {items}")
    visualize()
    save_to_file("quantum_random_ga")

