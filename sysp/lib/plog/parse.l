%{
#include <stdio.h>
#include "parse.tab.h"

int  zeilennr  = 1;

%}

L [ \t]+
F [0-9a-f]+(\.[0-9a-f]*)?
S [\"][^\"]*[\"]
K [a-zA-Z][a-zA-Z0-9]*
R {F}[ \t]*-[ \t]*{F}

%%
^{L}*\n                       { zeilennr++; } /* Leere Zeilen weglesen */
\n                            { zeilennr++; } /* Zeilenende weglesen   */
{L}                           ; /* Leer und Tabzeichen ueberlesen */

^"(--)"                       {strcpy(yylval.string,yytext);return(PROBED);    }
^"(**)"                       {strcpy(yylval.string,yytext);return(FROMCONFIG);}
^"(==)"                       {strcpy(yylval.string,yytext);return(DEFAULT);   }
^"(++)"                       {strcpy(yylval.string,yytext);return(CMDLINE);   }
^"(!!)"                       {strcpy(yylval.string,yytext);return(NOTICE);    }
^"(II)"                       {strcpy(yylval.string,yytext);return(INFO);      }
^"(WW)"                       {strcpy(yylval.string,yytext);return(WARNING);   }
^"(EE)"                       {strcpy(yylval.string,yytext);return(ERROR);     } 

[a-z0-9/][A-Z0-9/ ]+"("[0-9]+"):" {strcpy(yylval.string,yytext);return(ID);    }

"PCI ".*                      {strcpy(yylval.string,yytext);return(BUSID);     }
"Primary Device is: "         {strcpy(yylval.string,yytext);return(PDEV);      }
"VideoRAM"[ \t]*:             {strcpy(yylval.string,yytext);return(MEMORY);    }
"Mapped VideoRAM"[ \t]*:      {strcpy(yylval.string,yytext);return(MEMORY);    }
"probed videoram:"            {strcpy(yylval.string,yytext);return(MEMORY);    }
"hardware:".*"(vidmem:"       {strcpy(yylval.string,yytext);return(MEMORY);    }
"Will alloc AGP framebuffer:" {strcpy(yylval.string,yytext);return(MEMORY);    }
"kB of"                       {strcpy(yylval.string,yytext);return(MEMORY);    }
"VESA VBE Total Mem:"         {strcpy(yylval.string,yytext);return(MEMORY);    }
"vram:  "                     {strcpy(yylval.string,yytext);return(MEMORY);    }
"kBytes"                      {strcpy(yylval.string,yytext);return(UNIT);      }
"kByte"                       {strcpy(yylval.string,yytext);return(UNIT);      }
"mBytes"                      {strcpy(yylval.string,yytext);return(UNIT);      }
"Chipset"[ \t]*:              {strcpy(yylval.string,yytext);return(CHIPSET);   }
"VESA Video Modes"[ \t]*:     {strcpy(yylval.string,yytext);return(VESA);      }
{F}[xX]{F}@{F}Hz              {strcpy(yylval.string,yytext);return(VESAMODE);  }
"Max pixel clock is"          {strcpy(yylval.string,yytext);return(DAC);       }
"Maximum pixel clock"[ \t]*:  {strcpy(yylval.string,yytext);return(DAC);       }
"Maximum clock:"              {strcpy(yylval.string,yytext);return(DAC);       }
"clock range:".*"to"          {strcpy(yylval.string,yytext);return(DAC);       }
"Max Clock"[ \t]*:            {strcpy(yylval.string,yytext);return(DAC);       }
"pixel clocks"[ \t]*:         {strcpy(yylval.string,yytext);return(CLOCKS);    }
"refresh"[ \t]*:              {strcpy(yylval.string,yytext);return(REFRESH);   }
"panel is a"                  {strcpy(yylval.string,yytext);return(PANEL);     }
"panel"                       {strcpy(yylval.string,yytext);return(PANEL);     }
"TFT LCD panel detected"      {strcpy(yylval.string,yytext);return(PANEL);     }
"DSTN Panel"                  {strcpy(yylval.string,yytext);return(PANEL);     }
"TFT Panel Size ="            {strcpy(yylval.string,yytext);return(PANEL);     }
"TFT Panel"                   {strcpy(yylval.string,yytext);return(PANEL);     }
"display size"                {strcpy(yylval.string,yytext);return(PANEL);     }
{F}[xX]{F}                    {strcpy(yylval.string,yytext);return(RESO);      }
"Manufacturer:".*"Model:"     {strcpy(yylval.string,yytext);return(DDC);       }
"vmware_colordepth:"          {strcpy(yylval.string,yytext);return(VMWARE);    }
"depth:"                      {strcpy(yylval.string,yytext);return(VMWARE);    }
"Max H-Image Size [cm]: horiz.:"  {strcpy(yylval.string,yytext);return(DPIX);  }
"vert.:"                      {strcpy(yylval.string,yytext);return(DPIY);      }

{S}                           {strncpy(yylval.string,yytext,255);yylval.string[255]='\0';return(STRING);    }
{F}                           {strcpy(yylval.string,yytext);return(FLOAT);     }
.                             {strcpy(yylval.string,yytext);return(TRASH);     }

%%
