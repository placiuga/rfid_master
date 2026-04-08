from adminFunctions import setup_pn532, create_user, delete_user, executeSQL, editTables, authorizeNewUser
from rich import print
from rich.table import Table
from rich.console import Console
from rich.panel import Panel
from rich.align import Align
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

def exit_program():
    print("Exiting program. Goodbye!")
    
    exit()

def printMenu():
    console = Console()
    menuText = (
        "[bold #EE7624]Main Menu[/bold #EE7624]\n"
        "[bold]1: [/] Create user from tag\n"
        "[bold]2: [/] Execute raw SQL query (ADVANCED)\n"
        "[bold]3: [/] Delete user by tag\n"
        "[bold]4: [/] Edit database tables \n"
        "[bold]5: [/] Authorize new user with tag\n"
        "\n[bold]0: [/] Exit\n"
         )

    header = Panel(
        Align.center("[bold #EE7624]Main Menu[/bold #EE7624]"),
        border_style="#782F40",
        padding=(1, 4)
    )
    console.print(
        Panel(
            Align.left(menuText),
            border_style = "#782F40",
            padding = (1,4)
            )
    )

def get_menu_choice():
    while True:
        try:
            choice = int(input("Enter menu number: "))
            
            if choice in [0, 1, 2, 3, 4]:
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


while menu != 0:
    printMenu()
    menu = get_menu_choice()

    if menu == 0:
        exit_program()
    elif menu == 1:
        create_user(conn)
    elif menu == 2:
        executeSQL(conn)
    elif menu == 3:
        delete_user(conn)
    elif menu == 4:
        editTables(conn)
    elif menu == 5:
        authorizeNewUser(conn)


print("Exiting program. Goodbye!")
