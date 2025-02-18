}else{
                printf ("ccccccccc\n");
                for(int i=0 ; i<automate->countNode ; i++){
                    if(strcasecmp(token,automate->nodes[i].Id)==0){
                        char* Ids[50];
                        char* etiquettes[50] ;
                        int nbrID=0;
                        int nbrEtiquette=0;
                        token = strtok(NULL," [].,!}?= ->\n");
                        while(!(strcasecmp(token,"label")==0)){  
                            Ids[nbrID]=token;
                            nbrID++;
                            token = strtok(NULL," [].,!}?= ->\n");
                        }
                        token = strtok(NULL," [].,!}?= ->\n");
                        while(token!=