keyGenerator
(grade week 2 coursework extention) - Lucas O'Dowd-Jones
This program is used to generate a shared private key between 2 parties using the Diffie-Hellman asymettric encryption method (https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange).

The operation of the program is as follows:

One party generates a public key by running
$ ./keyGenerator [UPPER CASE PASSWORD]
from the terminal.

Another party does the same with their own password to generate their own public key (the 2 parties never share their indiviual passwords)

The 2 parties exchange public keys over an unsecured channel (however due to the the nature of the keys their passwords are not determineable from these keys).

Each party then executes the following in the terminal
$ .keyGenerator [UPPER CASE PASSWORD] [OPTIONAL - RECIEVED PUBLIC KEY]
to generate the same, shared private key known only by eachother which can be used as a basis for further encrypted communication using symettric encryption methods.

Executing
$ ./keyGenerator
without any arguments runs automated testing using the assert() function.

Resources used:
https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method - psudeocode adapted to implement modular exponentiation, explained well in the following Khan Academy article

Limitations:
Only a basic implementation - enterprise implementations use keys with at least 2048 bits.
Needs input validation on password and public keys.
