import board
import busio
import digitalio
from adafruit_pn532.spi import PN532_SPI
from rich import print
from rich.table import Table
from rich.console import Console
import mariadb
import sys

print(r"""
[bold #EE7624]
//////////////////////////////////////////////////////////////////////////////////
[/bold #EE7624]
[bold #782F40]

              .o88o.  o8o        .o8            .oooo.     .oooo.    .ooooo.   
              888 `"  `"'       "888          .dP""Y88b   d8P'`Y8b  888' `Y88. 
    oooo d8b o888oo  oooo   .oooo888                ]8P' 888    888 888    888 
    `888""8P  888    `888  d88' `888              <88b.  888    888  `Vbood888 
     888      888     888  888   888  8888888      `88b. 888    888       888' 
     888      888     888  888   888          o.   .88P  `88b  d88'     .88P'  
    d888b    o888o   o888o `Y8bod88P"         `8bd88P'    `Y8bd8P'    .oP'     
[/bold #782F40]
[bold #EE7624]
//////////////////////////////////////////////////////////////////////////////////
[/bold #EE7624]                                                                                        
Welcome to the RFID Lab Access admin panel!
""")
print("Attempting to connect to database...")


def connectToDB():
    username = input("Please enter your mariaDB username: ")
    pwd = input("Please enter your mariaDB password: ")
    try:
        conn = mariadb.connect(
            user = f"{username}",
            password= f"{pwd}",
            host = "127.0.0.1",
            port = 3306,
            database = "rfid"
            
            )
    except mariadb.Error as e:
        print(f"Error connecting to MariaDB Platform: {e}")
        sys.exit(1)
    cur = conn.cursor()
    print(f"Connection success! User {username} accessing local database 'rfid' ")
    return cur, conn

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

def executeSQL(conn):
    print("WARNING: This option allows you to execute raw SQL queries on the database. Use with caution!")
    print("Type 'cancel' to return to main menu.")
    query = input("SQL> ").strip()
    if query.lower() == 'cancel' or not query:
        print("Returning to main menu.")
        return

    try:
        cur = conn.cursor()
        cur.execute(query)
        if cur.description:
            cols = [desc[0] for desc in cur.description]
            rows = cur.fetchall()

            print(f"Query executed successfully. {len(rows)} rows returned.")
            table = Table(title = "Query Results")
            for col in cols:
                table.add_column(col)
            for row in rows:
                table.add_row(*[str(value) for value in row])

            console = Console()
            console.print(table)
            if not rows:
                print("(no rows returned)")

        else:
            conn.commit()
            print(f"\nQuery executed successfully. {cur.rowcount} row(s) affected.")


    except mariadb.Error as e:
        print(f"Error executing query: {e}")

def printMenu():
    print("1: Create user from tag")
    print("2: Execute raw SQL query (ADVANCED)")
    print("\n0: Exit\n")


def get_menu_choice():
    while True:
        try:
            choice = int(input("Enter menu number: "))
            
            if choice in [0, 1, 2]:
                return choice
            else:
                print("Invalid choice. \n")
                
        except ValueError:
            print("Invalid input. Please enter a number.\n")
        except EOFError:
            print("\nExiting.")
            return 0

menu = -1
cur, conn = connectToDB()
print("Current tables in databse:")
cur.execute("SHOW TABLES;")
for(table,) in cur:
    print(table)


while menu != 0:
    printMenu()
    menu = get_menu_choice()

    if menu == 0:
        exit_program()
    elif menu == 1:
        create_user()
    elif menu == 2:
        executeSQL(conn)


print("Exiting program. Goodbye!")
