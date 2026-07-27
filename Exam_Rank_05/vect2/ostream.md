the left operatand decides where a binary operator can live.
`a<<b` as a member fucntion is sugar for a.operator<<(b) - meaning tht the member must belong to the type of a, the left operand. Here the left operand is std::cout (a std::ostream), not our vect2, we don't own , we can't add a member to a class we don't cotrol. So operator<<(ostream&, vect2) has to be a free fucntion outside both classes, taking the stream by reference and our object by const reference. 

Contrastl with operator+: v1 + v2 -> left operand is `vect2`, so taht can be  a member (v2.operator+(v2))

contrast with operator+: v1 + v2 -< left operand is vect2, so that one can be a member (v1.operator+(v2))
Rule of thumb: if the left operand is our calss can be a member. If the left operand is someone else's type (like std::ostream or int in 3 * v2) -> must be a free function.

