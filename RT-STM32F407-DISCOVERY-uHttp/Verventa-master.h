/*! \file 	lcfgfile_data.h
	\brief	lcfg file system.
	
	Details.
 */
/*! \def 	lcfg files
	\brief	list of cfg files
	 
	Details.
 */
 
#define NUMBER_OF_FILES	2

/*! \ingroup lcfg
	* \brief main_cfg
 *
 * \param list	version																Major.Minor.Release		EOF
 */
const unsigned char main_cfg[]={'v', 'e', 'r', 's', 'i', 'o', 'n', ' ', '=', ' ', '"', '1', '0', '1', '"', '\0'};

/*! \ingroup lcfg
	* \brief modbus_cfg
 *
 * \param list	address, 
 */
const unsigned char modbus_cfg[]={'A', 'D', 'D', 'R', 'E', 'S', 'S', '=', '"', '1', '0', '"', '\n',
								'N', 'A', 'M', 'E', '=', '"', 'R', 'T', 'C', '"', '\n',
								'V', 'A', 'L', 'V', 'E', 'S', '=', '[', '"', 'V', 'A', 'L', 'V', 'E', '1', '"',
								',', '"', 'V', 'A', 'L', 'V', 'E', '2', '"', ']', '\0'};
/*const unsigned char modbus_cfg[]={'V', 'A', 'L', 'V', 'E', 'S', '=', '[', '"', 'V', 'A', 'L', 'V', 'E', '1', '"',
									',', '"', 'v', 'A', 'L', 'V', 'E', '2', '"', ']', '\0'};*/

/*! \ingroup lcfg
 * \brief lcfg_file struct
 *
 * \param *name
 * \param *data
 */
struct lcfg_file
{
	const unsigned char *name;
	const unsigned char *data;
}const lcfg_table[NUMBER_OF_FILES]={
								{"main.cfg", &main_cfg[0]},
								{"modbus.cfg", &modbus_cfg[0]}
							 };

