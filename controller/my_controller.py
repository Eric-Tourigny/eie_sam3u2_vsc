from openant.easy.node import Node
from openant.easy.channel import Channel

import threading
import time

# Definition of Variables
NETWORK_KEY = [0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0]
DEVICE_TYPE = 1
TRANSMISSION_TYPE = 1
DEVICE_NUMBER = 5666  # Change if you need.
CHANNEL_PERIOD = 4096
CHANNEL_FREQUENCY = 50


class GetInput:
    def __init__(self):
        self.count = 5


    def await_input(self):
        while(1):
            user_input = input()
            self.count = (self.count + 1) % 256


    def get_input(self):
        return self.count


USERINPUT = GetInput()

class ANTSend:
    def __init__(self):
        self.ANTMessagePayload = [0] * 8
        self.last_time = time.time()

    def open_channel(self):
        self.node = Node()
        self.node.set_network_key(0x00, NETWORK_KEY)

        self.channel = self.node.new_channel(Channel.Type.BIDIRECTIONAL_TRANSMIT, 0, 0)
        self.channel.set_id(DEVICE_NUMBER, DEVICE_TYPE, TRANSMISSION_TYPE)
        self.channel.set_period(CHANNEL_PERIOD)
        self.channel.set_rf_freq(CHANNEL_FREQUENCY)
        self.channel.on_broadcast_tx_data = self.on_event_tx

        try:
            self.channel.open()
            self.node.start()
        except KeyboardInterrupt:
            print("Closing ANT Channel")
            self.channel.close()
            self.node.stop()

    def set_data(self):
        self.ANTMessagePayload[0] = USERINPUT.get_input()

    # Callback function for event TX
    def on_event_tx(self, data):
        self.set_data()
        self.channel.send_broadcast_data(self.ANTMessagePayload)
        print(f"{time.time() - self.last_time :.5f} TX: {DEVICE_NUMBER}, {DEVICE_TYPE} : {self.ANTMessagePayload}")
        self.last_time = time.time()



##########################################################################################################


def main():
    input_thread = threading.Thread(target=USERINPUT.await_input)
    input_thread.start()

    ant_sender = ANTSend()
    ant_sender.open_channel()
    


if __name__ == "__main__": 
    main()