#ifndef PARSER_H_
#define PARSER_H_

struct image;

FILE *open_for_parse(char *path);
void close_file(FILE *f);
void parse_file(char *path, struct camera **c, struct shape_list **sl,
		struct light_list **ll, struct image *img, unsigned *nbcore,
		double *anti_cr, unsigned *fps, unsigned *sec);

#endif /* PARSER_H_ */
