#ifndef USER_MODE_H
#define USER_MODE_H

/*
 * Declaração da função em Assembly que forja o retorno 
 * de interrupção para saltar para o Ring 3 (User Mode).
 */
extern void enter_user_mode(void);

#endif /* USER_MODE_H */