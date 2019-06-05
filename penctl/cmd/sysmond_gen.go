// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.
/*
 * Package cmd is a auto generated package.
 * Input file: sysmond.proto
 */

package cmd

import (
	"fmt"

	"github.com/spf13/cobra"
)

//cmd - frequency
//rootCmd = system
//helpStr = System power information:\n\n\nValue Description:\n\nfrequency: Frequency of the system.\nThe frequency is MHz\n
var frequencysystemShowCmd = &cobra.Command{
	Use:   "frequency",
	Short: "System power information:\n\n\nValue Description:\n\nfrequency: Frequency of the system.\nThe frequency is MHz\n",
	Long:  "\n---------------------------------\n System power information:\n\n\nValue Description:\n\nfrequency: Frequency of the system.\nThe frequency is MHz\n\n---------------------------------\n",
	RunE:  frequencysystemShowCmdHandler,
}

func frequencysystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicfrequencymetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicfrequencymetrics object(s) found")
	}
	return nil
}

//cmd - memory
//rootCmd = system
//helpStr = System memory information:\n\n\nValue Description:\n\nTotal Memory: Total Memory of the system.\nAvailable Memory: Available Memory of the system.\nFree Memory: Free Memory of the system.\nThe memory is KB\n
var memorysystemShowCmd = &cobra.Command{
	Use:   "memory",
	Short: "System memory information:\n\n\nValue Description:\n\nTotal Memory: Total Memory of the system.\nAvailable Memory: Available Memory of the system.\nFree Memory: Free Memory of the system.\nThe memory is KB\n",
	Long:  "\n---------------------------------\n System memory information:\n\n\nValue Description:\n\nTotal Memory: Total Memory of the system.\nAvailable Memory: Available Memory of the system.\nFree Memory: Free Memory of the system.\nThe memory is KB\n\n---------------------------------\n",
	RunE:  memorysystemShowCmdHandler,
}

func memorysystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicmemorymetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicmemorymetrics object(s) found")
	}
	return nil
}

//cmd - power
//rootCmd = system
//helpStr = System power information:\n\n\nValue Description:\n\npin: Input power to the system.\npout1: Core output power.\npout2: Arm output power.\nThe power is milli Watt\n
var powersystemShowCmd = &cobra.Command{
	Use:   "power",
	Short: "System power information:\n\n\nValue Description:\n\npin: Input power to the system.\npout1: Core output power.\npout2: Arm output power.\nThe power is milli Watt\n",
	Long:  "\n---------------------------------\n System power information:\n\n\nValue Description:\n\npin: Input power to the system.\npout1: Core output power.\npout2: Arm output power.\nThe power is milli Watt\n\n---------------------------------\n",
	RunE:  powersystemShowCmdHandler,
}

func powersystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicpowermetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicpowermetrics object(s) found")
	}
	return nil
}

//cmd - temp
//rootCmd = system
//helpStr = System temperature information:\n\n\nValue Description:\n\nlocal_temperature: Temperature of the board.\ndie_temperature: Temperature of the die.\nhbm_temperature: Temperature of the hbm.\nThe temperature is degree Celcius\n
var tempsystemShowCmd = &cobra.Command{
	Use:   "temp",
	Short: "System temperature information:\n\n\nValue Description:\n\nlocal_temperature: Temperature of the board.\ndie_temperature: Temperature of the die.\nhbm_temperature: Temperature of the hbm.\nThe temperature is degree Celcius\n",
	Long:  "\n---------------------------------\n System temperature information:\n\n\nValue Description:\n\nlocal_temperature: Temperature of the board.\ndie_temperature: Temperature of the die.\nhbm_temperature: Temperature of the hbm.\nThe temperature is degree Celcius\n\n---------------------------------\n",
	RunE:  tempsystemShowCmdHandler,
}

func tempsystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asictemperaturemetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asictemperaturemetrics object(s) found")
	}
	return nil
}

//cmd - system
//rootCmd =
//helpStr = Metrics for system monitors
var systemShowCmd = &cobra.Command{
	Use:   "system",
	Short: "Metrics for system monitors",
	Long:  "\n---------------------------------\n Metrics for system monitors\n---------------------------------\n",
}

func init() {

	systemShowCmd.AddCommand(frequencysystemShowCmd)

	systemShowCmd.AddCommand(memorysystemShowCmd)

	systemShowCmd.AddCommand(powersystemShowCmd)

	systemShowCmd.AddCommand(tempsystemShowCmd)

	//cmd - system
	//rootCmd =
	//helpStr = Metrics for system monitors

	metricsShowCmd.AddCommand(systemShowCmd)

}
