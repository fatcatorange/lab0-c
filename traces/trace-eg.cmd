# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show
# Create empty queue
new
# See how long it is
size
# Fill it with some values.  First at the head
ih abc
ih def
ih ssj
ih jimmy
ih jimmy 
ih immy
# Now at the tail
it meerkat
it bear
it jimmy
# sortA
sort
# dedup
dedup
# swap
swap
# Reverse it
reverse
# See how long it is
size
# Delete queue.  Goes back to a NULL queue.
free
# Exit program
quit
