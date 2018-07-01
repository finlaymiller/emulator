<<<<<<< HEAD
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU mainline
- Finlay Miller B00675696
- 01 JULY 2018
*/

void fetch(void)
{
    MAR = regfile[PC];

    // read from PC & store in instruction register
    bus(MAR, MDR, WORD, READ);
    IR = MDR;

    printf("\nFetching memory address: 0x%x\t|\tcontents: 0x%x\n", MAR, MDR);

    regfile[PC] += 2;
}

void decode(void)
{
    enum instType decodetype = decode_type();
    iMnemonic = decode_mnemonic(decodetype);
}

void execute (void)
{
    printf("\n");
    
    	switch (iMnemonic) {
	case LD:
		printf("Decoded: LD\n");
		//executeLD();
		break;
	case ST:
		printf("Decoded: ST\n");
		//executeST();
		break;
	case LDR:
		printf("Decoded: LDR\n");
		//executeLDR();
		break;
	case STR:
		printf("Decoded: STR\n");
		//executeSTR();
		break;
	case MOVL:
		printf("Decoded: MOVL\n");
		//executeMOVL();
		break;
	case MOVH:
		printf("Decoded: MOVH\n");
		//executeMOVH();
		break;
	case MOVLZ:
		printf("Decoded: MOVLZ\n");
		//executeMOVLZ();
		break;
	case BL:
		printf("Decoded: BL\n");
		//executeBL();
		break;
	case BEQ:
		printf("Decoded: BEQ\n");
		//executeBEQ();
		break;
	case BNE:
		printf("Decoded: BNE\n");
		//executeBNE();
		break;
	case BC:
		printf("Decoded: BC\n");
		//executeBC();
		break;
	case BNC:
		printf("Decoded: BNC\n");
		//executeBNC();
		break;
	case BN:
		printf("Decoded: BN\n");
		//executeBN();
		break;
	case BGE:
		printf("Decoded: BGE\n");
		//executeBGE();
		break;
	case BLT:
		printf("Decoded: BLT\n");
		//executeBLT();
		break;
	case BAL:
		printf("Decoded: BAL\n");
		//executeBAL();
		break;
	case ADD:
		printf("Decoded: ADD\n");
		//executeADD();
		break;
	case ADDC:
		printf("Decoded: ADDC\n");
		//executeADDC();
		break;
	case SUB:
		printf("Decoded SUB\n");
		//executeSUB();
		break;
	case SUBC:
		printf("Decoded: SUBC\n");
		//executeSUBC();
		break;
	case DADD:
		printf("Decoded: DADD\n");
		//executeDADD();
		break;
	case CMP:
		printf("Decoded: CMP\n");
		//executeCMP();
		break;
	case XOR:
		printf("Decoded: XOR\n");
		//executeXOR();
		break;
	case AND:
		printf("Decoded: AND\n");
		//executeAND();
		break;
	case BIT:
		printf("Decoded: BIT\n");
		//executeBIT();
		break;
	case BIC:
		printf("Decoded: BIC\n");
		//executeBIC();
		break;
	case BIS:
		printf("Decoded: BIS\n");
		//executeBIS();
		break;
	case MOV:
		printf("Decoded: MOV\n");
		//executeMOV();
		break;
	case SWAP:
		printf("Decoded: SWAP\n");
		//executeSWAP();
		break;
	case SRA:
		printf("Decoded: SRA\n");
		//executeSRA();
		break;
	case RRC:
		printf("Decoded: RRC\n");
		//executeRRC();
		break;
	case SWPB:
		printf("Decoded: SWPB\n");
		//executeSWPB();
		break;
	case SXT:
		printf("Decoded: SXT\n");
		//executeSXT();
		break;
	default:
		printf("ERROR\n");
		break;
	}
}

enum instType decode_type(void)
{

    if (CHECK_BIT(IR, 15)
    {
        // decode LDST & MOV type instructions 
        if (!CHECK_BIT(IR, 14) && CHECK_BIT(IR, 13))
        {
            return MOV;
        }
        else if (!CHECK_BIT(IR, 14) && CHECK_BIT(IR, 12))
        {
            return MOV;
        }
        else return LDST;
    }
    else 
    {
        if (!CHECK_BIT(IR, 14))
        {
            // identify branching instructions
            return BRANCHING;
        }
        else
        {
            if (CHECK_BIT(IR, 8))
            {
                // identify register control instructions
                return REGCON;
            }
            else return ALU;
        }
       
    }

    return;
}

enum instMnem decode_mnemonic(enum instType type)
{
    enum instMnem mnemonic;

    return mnemonic;
}

enum instMnem decode_LDST(void)
{
    if (CHECK_BIT(IR, 14))      // 1X
    {
        if (CHECK_BIT(IR, 13))  // 11X
            return STR;         // 111
        else return LDR;        // 110
    }
    else
    {
        if (CHECK_BIT(IR, 11))  // 1000X
            return ST;          // 10001
        else return LD;         // 10000
    }
}

enum instMnem decode_MOV(void)
{
    if (CHECK_BIT(IR, 13))      // 10X
    {
        return MOVH;            // 101
    }
    else
    {
        if (CHECK_BIT(IR, 11))  // 1001X
            return MOVLZ;       // 10011
        else return MOVL;       // 10010
    }
}

enum instMnem decode_BRANCHING(void)
{
	if (CHECK_BIT(IR, 13)) {
		//bit 13 set

		if (CHECK_BIT(IR, 12)) {
			//bit 12 set

			if (CHECK_BIT(IR, 11)) {
				//bit 11 set

				if (CHECK_BIT(IR, 10)) {
					//bit 10 set
					return BAL;
				}
				else {
					//bit 10 not set
					return BLT;
				}
			}
			else {
				//bit 11 not set

				if (CHECK_BIT(IR, 10)) {
					//bit 10 set
					return BGE;
				}
				else {
					//bit 10 not set
					return BN;
				}
			}
		}
		else {
			//bit 12 not set
			
			if (CHECK_BIT(IR, 11)) {
				//bit 11 set

				if (CHECK_BIT(IR, 10)) {
					//bit 10 set
					return BNC;
				}
				else {
					//bit 10 not set
					return BC;
				}
			}
			else {
				//bit 11 not set

				if (CHECK_BIT(IR, 10)) {
					//bit 10 set
					return BNE;
				}
				else {
					//bit 10 not set
					return BEQ;
				}
			}
		}
	}
	else
	{
		//bit 13 not set
		return BL;
	}
}

enum instMnem decode_ALU(void)
{
    	if (CHECK_BIT(IR, 12)) {
		//bit 12 set

		if (CHECK_BIT(IR, 11)) {
			//bit 11 set
			return SWAP;
		}
		else {
			//bit 11 not set
			
			if (CHECK_BIT(IR, 10)) {
				//bit 10 set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return MOV;
				}
				else {
					//bit 9 not set
					return BIS;
				}
			}
			else {
				//bit 10 not set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return BIC;
				}
				else {
					//bit 9 not set
					return BIT;
				}
			}
		}
	}
	else {
		//bit 12 not set

		if (CHECK_BIT(IR, 11)) {
			//bit 11 set

			if (CHECK_BIT(IR, 10)) {
				//bit 10 set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return AND;
				}
				else {
					//bit 9 not set
					return XOR;
				}
			}
			else {
				//bit 10 not set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return CMP;
				}
				else {
					//bit 9 not set
					return DADD;
				}
			}
		}
		else {
			//bit 11 not set

			if (CHECK_BIT(IR, 10)) {
				//bit 10 set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return SUBC;
				}
				else {
					//bit 9 not set
					return SUB;
				}
			}
			else {
				//bit 10 not set

				if (CHECK_BIT(IR, 9)) {
					//bit 9 set
					return ADDC;
				}
				else {
					//bit 9 not set
					return ADD;
				}
			}
		}
	}
}

enum instMnem decode_REGCON(void)
{
    if (CHECK_BIT(IR, 10)) {
		//bit 10 set

		if (CHECK_BIT(IR, 9)) {
			//bit 9 set
			return SXT;
		}
		else {
			//bit 9 not set
			return SWPB;
		}
	}
	else {
		//bit 10 not set

		if (CHECK_BIT(IR, 9)) {
			//bit 9 set
			return RRC;
		}
		else {
			//bit 9 not set
			return SRA;
		}
	}
=======
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU mainline
- Finlay Miller B00675696
- 29 June 2018
*/

void fetch(void)
{

>>>>>>> 278da0870bc211b7e3ccb4e0cda1249e0ec582e5
}