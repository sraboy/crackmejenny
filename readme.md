# Crackme Jenny

A not-terribly-complicated crackme with a couple interesting twists. There are three versions available in the **bin** directory:

1. Easy mode (developer): unstripped binary, includes a bunch of printf statements providing debug output
2. Medium mode (debug): unstripped binary, no debug output
3. Hard mode (release): stripped, no help at all

# Scenario

You're a consultant being hired to reverse engineer this key generator. Your client has the activation key (13284645-2427364-104376652) but lost the username and password used to generate that key in a recent datacenter catastrophe.

The original developer has gone out of business and the current owner of the online portal claims to have no access to the prior company's records and will not help. The username and password is required to access the online portal, which contains years of business records for your client.

Your client doesn't like the idea of cracking the original software, for legal reasons, so it is your job to discover the username and password that generated this key.

No one knows how this software works. It was used years ago to generate the key but that was all done by sub-contractors who are long gone.

# License

This project is released under the MIT license.