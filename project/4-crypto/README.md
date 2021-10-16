# 4 - Crypto

Task: given the /etc/shadow user entry with SHA512 hashed password, get the user's actual password

## Hint
User likes country music

## Intuition
Hash algorithm works only one way (no way to decrypt from hash itself). One way is brute forcing a password until a match is found.
Given the hint, we can guess that the password is going to be a name of a country music artist.
So we take the list of artist names from a [Wikipedia list of country music performers](https://en.wikipedia.org/wiki/List_of_country_music_performers) and parse it using some bash tools to derive a brute force dictionary.
Then we iterate over it using the original name, lowercase and camelcase versions of it, generate a hash over each and compare with result

## Implementation

Files:
- __dictionary.txt__: brute force dictionary of artist names generated using the methods decribed in the Intuition section
- __decrypt.sh__: bash script which generates the proper hashes and compares with the user /etc/shadow entry
- __entry.txt__: user's /etc/shadow entry

Command:
``` bash
./decrypt.sh dictionary.txt $(cat entry.txt)
```