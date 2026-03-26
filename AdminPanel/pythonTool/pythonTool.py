import board
import busio
import digitalio
from adafruit_pn532.spi import PN532_SPI

print(r"""
//////////////////////////////////////////////////////////////////////////////////

              .o88o.  o8o        .o8            .oooo.     .oooo.    .ooooo.   
              888 `"  `"'       "888          .dP""Y88b   d8P'`Y8b  888' `Y88. 
    oooo d8b o888oo  oooo   .oooo888                ]8P' 888    888 888    888 
    `888""8P  888    `888  d88' `888              <88b.  888    888  `Vbood888 
     888      888     888  888   888  8888888      `88b. 888    888       888' 
     888      888     888  888   888          o.   .88P  `88b  d88'     .88P'  
    d888b    o888o   o888o `Y8bod88P"         `8bd88P'    `Y8bd8P'    .oP'     

//////////////////////////////////////////////////////////////////////////////////
                                                                                          
Welcome to the RFID Lab Access admin panel!
""")

def setup_pn532():
    spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
    cs = digitalio.DigitalInOut(board.D25) ##Pin 22 on the Pi
    pn532 = PN532_SPI(spi, cs, debug=False)
    ic, ver, rev, support = pn532.firmware_version
    print("Found PN532: ver={ver}.{rev}, support={support}".format(ver=ver, rev=rev, support=support))
    pn532.SAM_configuration()
    return pn532

def exit_program():
    print("Exiting program. Goodbye!")
    exit()

def create_user():
    print("Configuring PN532 RFID reader...")
    pn532 = setup_pn532()
    print("Waiting for tag...")
    while True:
        uid = pn532.read_passive_target(timeout=1.0)
        if uid is not None:
            print("Tag detected! UID:", [hex(i) for i in uid])
            break
    uidStr = ''.join(f'{i:02X}' for i in uid) ##Convert UID to hex string (join i in uid with uppercase hex, pad to 2 chars to add leading zero)
    name = input("Enter student name: ")
    userID = input("TEMP: Enter PK UserID: ")
    FSUID = input("Enter FSUID: ")
    status = input("Enter status (active/inactive): ")
    print(f"Creating user with name: {name}, userID: {userID}, FSUID: {FSUID}, status: {status}")
    query = f"INSERT INTO UserTable (Name, UserID, FSUID, Status, rfid_uid) VALUES ('{name}', '{userID}', '{FSUID}', '{status}', '{uidStr}')"
    print("Executing query:", query)
    print("Success! User created.")

def printMenu():
    print("1: Create user from tag")
    print("\n0: Exit\n")

menuActions = {
    0: exit_program,
    1: create_user
    }


def get_menu_choice():
    while True:
        try:
            choice = int(input("Enter menu number: "))
            
            if choice in [0, 1]:
                return choice
            else:
                print("Invalid choice. Enter 0 or 1.\n")
                
        except ValueError:
            print("Invalid input. Please enter a number.\n")
        except EOFError:
            print("\nExiting.")
            return 0

menu = -1
while menu != 0:
    printMenu()
    menu = get_menu_choice()

    action = menuActions.get(menu)
    if action:
        action()


print("Exiting program. Goodbye!")
