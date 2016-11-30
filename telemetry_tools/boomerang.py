# boomerang.py
# This script forwards serial port telemetry over UDP in a broadcast fashion
# (c) 2016, Abhimanyu Ghosh
import os
import serial
import struct
import argparse
import socket

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation

class UDP_Boomerang:
	def __init__(self, udp_port, serportm, serbaud):
		self.udp_port = udp_port
		self.serport = serial.Serial(serport, serbaud)

	def send_packet(self, data):
		ip = '255.255.255.255'
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		sock.sendto(data, (ip, self.udp_port))

	def redirect_telemetry_message(self):
		message_buffer = ""
		start = self.serport.read(1)
		if start == 's':
			message_buffer += start

			desc_size_byte = self.serport.read(1) 
			desc_size = ord(desc_size_byte)
			descriptor = self.serport.read(desc_size)

			message_buffer += desc_size_byte
			message_buffer += descriptor

			msg_id_byte = self.serport.read(1) 
			msg_id = ord(msg_id_byte)

			message_buffer += msg_id_byte

			if(msg_id != 3 && msg_id != 4): # I.e. not matrix transfers
				data_len_byte = self.serport.read(1) 
				data_len = ord( data_len_byte)

				message_buffer += data_len_byte

				msg_payload = self.serport.read(data_len)
				chksum = self.serport.read(2)

				message_buffer += msg_payload
				message_buffer += chksum

			else:
				rows_byte = self.serport.read(1)
				cols_byte = self.serport.read(1)
				msg_payload = self.serport.read(ord(rows_byte)*ord(cols_byte))
				chksum = self.serport.read(2)

				message_buffer += rows_byte
				message_buffer += cols_byte
				message_buffer += msg_payload
				message_buffer += chksum

			self.send_packet(message_buffer)

def main():
	parser = argparse.ArgumentParser(description='Redirect telemetry from serial port specified to UDP stream')
	parser.add_argument('baud', metavar='baudrate', type=int, nargs=1, help='Set serial port baud rate')
	parser.add_argument('serialport', metavar='serial_port', nargs=1, help='Path to serial port that is source of telemetry')
	parser.add_argument('udp_port', metavar='UDP_Portnum', type=int, nargs=1, help='UDP port to broadcast telemetry over')

	args = parser.parse_args()

	serial_baud_rate = args.baud[0]
	serial_port = args.serialport[0]
	udp_port = args.udp_port[0]

	bouncer = UDP_Boomerang(udp_port, serial_port, serial_baud_rate)

if __name__ == '__main__':
	main()