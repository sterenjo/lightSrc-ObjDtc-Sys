import communication
import utils

# object_locations = []  # TODO - bonus
# ldr_measures = []  # TODO - bonus

# Define the state mapping dictionary
state_mapping = {  # TODO - align values
    'state_0': '0',
    'state_1': 's',
    'state_2': 'o',
    'state_3': 't',
    'state_4': 'l',
    'state_5': 'b',
    'state_6': 'e'
}

def main():
    communication.init_com('COM7')
    utils.print_menu()


if __name__ == '__main__':
    main()
