//-----------------------------------------------------------------------------
// {C} Copyright 2017 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------

package cmd

import (
	"errors"
	"fmt"
	"strings"

	"github.com/spf13/cobra"

	"github.com/pensando/sw/venice/globals"
)

var logsShowCmd = &cobra.Command{
	Use:   "logs",
	Short: "Show logs from Naples",
	Long:  "\n------------------------------\n Show Module Logs From Naples \n------------------------------\n",
	Run:   logsShowCmdHandler,
	Args:  logsShowCmdArgsValidator,
}

var module string

func init() {
	rootCmd.AddCommand(logsShowCmd)

	str := "\t\t\tValid modules are:\n"
	str += "\t\t\t\t" + strings.TrimPrefix(globals.Nmd, "pen-") + "\n"
	str += "\t\t\t\t" + strings.TrimPrefix(globals.Netagent, "pen-") + "\n"
	str += "\t\t\t\t" + strings.TrimPrefix(globals.Tmagent, "pen-") + "\n"
	logsShowCmd.Flags().StringVarP(&module, "module", "m", "", "Module to show logs for\n"+str)
	logsShowCmd.MarkFlagRequired("module")
}

func logsShowCmdHandler(cmd *cobra.Command, args []string) {
	var moduleVal string
	switch module {
	case strings.TrimPrefix(globals.Nmd, "pen-"):
		moduleVal = globals.Nmd
	case strings.TrimPrefix(globals.Netagent, "pen-"):
		moduleVal = globals.Netagent
	case strings.TrimPrefix(globals.Tmagent, "pen-"):
		moduleVal = globals.Tmagent
	}
	moduleVal += ".log"
	resp, _ := restGet(revProxyPort, "monitoring/v1/naples/logs/"+moduleVal)
	fmt.Println(string(resp))
	if jsonFormat {
		fmt.Println("JSON not supported for this command")
	}
	if yamlFormat {
		fmt.Println("YAML not supported for this command")
	}
}

func logsShowCmdArgsValidator(cmd *cobra.Command, args []string) error {
	if strings.Compare(module, strings.TrimPrefix(globals.Nmd, "pen-")) != 0 &&
		strings.Compare(module, strings.TrimPrefix(globals.Netagent, "pen-")) != 0 &&
		strings.Compare(module, strings.TrimPrefix(globals.Tmagent, "pen-")) != 0 {
		if cmd.Flags().Changed("module") {
			str := "Not valid module: " + module + "\n"
			return errors.New(str)
		}
	}
	return nil
}
