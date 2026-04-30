def main():
    print(count_nodes_edge(process_dataset()))


def process_dataset():
    count = 0
    pre_list = []
    ordered_list = []
    undirected_graph = {}

    with open("roadNet-PA.txt", "r") as file:
        for line in file:
            if count == 20:
                break

            if line[0].isnumeric():
                format_line = line.replace("\t", " ").replace("\n", "")

                nodes = format_line.split(" ")

                try:
                    pre_list.append(tuple(sorted(map(int, nodes))))
                except ValueError:
                    print("Error en ingreso a tuple list")

            count += 1

        ordered_list = sorted(pre_list, key=lambda pre_list: pre_list[0])
        undirected_graph = set(ordered_list)

        # ---DEBUG
        for i in range(len(ordered_list)):
            print(
                ordered_list[i],
                ordered_list[i][0],
            )
        print(len(ordered_list), len(undirected_graph))
        # ---END DEBUG
    return undirected_graph


def count_nodes_edge(undirected_graph):
    number_of_nodes = set()
    for tuple in undirected_graph:
        for n in tuple:
            number_of_nodes.add(n)

    return (len(number_of_nodes), len(undirected_graph))


def create_processed_dataset(undirected_graph):
    with open("roadNet-PA-processed", "w") as final_file:
        ...


main()
