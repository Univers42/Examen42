#ifndef VBC_H
# define VBC_H

void	set_error(char c, int *err);

int	parse_expr(char **s, int *err);
int	parse_term(char **s, int *err);
int	parse_factor(char **s, int *err);

#endif
