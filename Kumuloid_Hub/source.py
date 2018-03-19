# This project requires PyBluez
from Tkinter import *
import bluetooth

root = Tk()
color_one = StringVar()
color_one.set("0xFF0000")
color_two = StringVar()
color_two.set("0x0000FF")
globalBrightness = StringVar()
globalBrightness.set("100")

nearby_devices = bluetooth.discover_devices()
# Run through all the devices found and list their name
# num = 0
# print "Select your device by entering its coresponding number..."
# for i in nearby_devices:
#     num += 1
#     print num, ": ", bluetooth.lookup_name(i)

# Allow the user to select their Arduino
# bluetooth module. They must have paired
# it before hand.
# selection = input("> ") - 1
# print "You have selected", bluetooth.lookup_name(nearby_devices[selection])

btName = "KumuloidCloud"

num = 0
didFind = 0
for i in nearby_devices:
    if bluetooth.lookup_name(i) == btName:
        didFind = 1
        break
    num += 1
if didFind == 0:
    print "Error, Cannot find cloud. Exiting..."
    exit()
bd_addr = nearby_devices[num]

port = 1


# Create the GUI
class Application(Frame):
    # Create a connection to the socket for Bluetooth
    # communication
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    def change(self):
        # Send 'a' which the Arduino
        # blink
        global color_one
        global color_two
        global globalBrightness
        color_one_hex = 0xFF0000
        color_two_hex = 0x0000FF
        global_brightness_hex = 100
        try:
            color_one_hex = int(color_one.get(), 16)
            color_two_hex = int(color_two.get(), 16)
            global_brightness_hex = int(globalBrightness.get(), 10)
        except Exception:
            print "There was an error processing this request..."
        # Because I was an idiot and programmed the cloud to work GRB instead of RGB
        # I have to fix it here

        if 0x0 > color_one_hex > 0xFFFFFF:
            color_one_hex = 0xFFFFFF
        if 0x0 > color_two_hex > 0xFFFFFF:
            color_two_hex = 0xFFFFFF
        if 0x0 > global_brightness_hex > 0xFF:
            global_brightness_hex = 100

        color_one_hex = ((color_one_hex & 0x00FF00) << 8) + \
                        ((color_one_hex & 0xFF0000) >> 8) + \
                        (color_one_hex & 0x0000FF)

        color_two_hex = ((color_two_hex & 0x00FF00) << 8) + \
                        ((color_two_hex & 0xFF0000) >> 8) + \
                        (color_two_hex & 0x0000FF)
        print "Color One: " + hex(color_one_hex)
        print "Color Two: " + hex(color_two_hex)
        print "Brightness: " + str(global_brightness_hex)
        word = (color_one_hex << 32) + (color_two_hex << 8) + global_brightness_hex
        converter = 48
        strgWord = ""
        while converter > -8:
            strgWord += chr((word & (0xFF << converter)) >> converter)
            converter -= 8

        print "Sending: " + strgWord
        data = "Q"
        self.sock.send(data)
        self.sock.send(strgWord)

    def rain(self):
        data = "a"
        self.sock.send("x")
        self.sock.send(data)

    def breath(self):
        data = "b"
        self.sock.send("x")
        self.sock.send(data)

    def walk(self):
        data = "c"
        self.sock.send("x")
        self.sock.send(data)

    def static(self):
        data = "d"
        self.sock.send("x")
        self.sock.send(data)

    def createWidgets(self):

        self.chooseRain = Button(self)
        self.chooseRain["text"] = "Rain",
        self.chooseRain["fg"] = "blue"
        self.chooseRain["command"] = self.rain

        self.chooseRain.pack({"side": "left"})

        self.chooseBreath = Button(self)
        self.chooseBreath["text"] = "Breath",
        self.chooseBreath["fg"] = "blue"
        self.chooseBreath["command"] = self.breath

        self.chooseBreath.pack({"side": "left"})

        self.chooseWalk = Button(self)
        self.chooseWalk["text"] = "Walk",
        self.chooseWalk["fg"] = "blue"
        self.chooseWalk["command"] = self.walk

        self.chooseWalk.pack({"side": "left"})

        self.chooseStatic = Button(self)
        self.chooseStatic["text"] = "Static",
        self.chooseStatic["fg"] = "blue"
        self.chooseStatic["command"] = self.static

        self.chooseStatic.pack({"side": "left"})

        self.c1Label = Label(self, text="-Color One-").pack()
        self.entryThing = Entry(self,
                                textvariable=color_one).pack()

        self.c2Label = Label(self, text="-Color Two-").pack()
        self.entryThing2 = Entry(self,
                                 textvariable=color_two).pack()

        self.brightLabel = Label(self, text="-Brightness-"
                                            "\nEnter 1 - 255").pack()
        self.entryThing2 = Entry(self,
                                 textvariable=globalBrightness).pack()

        self.submitStuff = Button(self, text="SUBMIT",
                                  fg="white", bg="red", command=self.change).pack()

    def __init__(self, master=None):
        # Connect to the bluetooth device
        # and initialize the GUI
        self.sock.connect((bd_addr, port))
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()


# Begin the GUI processing
app = Application(master=root)
app.mainloop()
root.destroy()
