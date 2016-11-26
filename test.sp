(
_growl "./growl.so" fl

_sine 4096 gen_sine

36 mtof 0.2 0.125 square 

# 5:4 lissajous curve
2.5 1 0 _sine osc 0 1 biscale 
2.2 1 0 _sine osc 0.5 1 biscale 
_growl fe 

# Add a line in the beginning to avoid click caused by filter
tick 0 0.03 1 line * 

_growl fc
)

