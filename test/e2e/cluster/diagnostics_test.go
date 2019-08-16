package cluster

import (
	"errors"
	"fmt"
	"strings"
	"time"

	"github.com/pensando/sw/api/generated/apiclient"

	. "github.com/onsi/ginkgo"
	. "github.com/onsi/gomega"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/api/fields"
	"github.com/pensando/sw/api/generated/audit"
	"github.com/pensando/sw/api/generated/auth"
	"github.com/pensando/sw/api/generated/diagnostics"
	"github.com/pensando/sw/api/generated/search"
	testutils "github.com/pensando/sw/test/utils"
	"github.com/pensando/sw/venice/apigw/svc"
	"github.com/pensando/sw/venice/globals"
)

var _ = Describe("diagnostics tests", func() {
	Context("spyglass logs", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.Spyglass)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.Spyglass)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// wait for spyglass to receive watch event
			time.Sleep(3 * time.Second)
			// create debug logs by searching for successful login audit event
			query := &search.SearchRequest{
				Query: &search.SearchQuery{
					Kinds: []string{auth.Permission_AuditEvent.String()},
					Fields: &fields.Selector{
						Requirements: []*fields.Requirement{
							{
								Key:      "action",
								Operator: "equals",
								Values:   []string{svc.LoginAction},
							},
							{
								Key:      "outcome",
								Operator: "equals",
								Values:   []string{audit.Outcome_Success.String()},
							},
							{
								Key:      "resource.kind",
								Operator: "equals",
								Values:   []string{string(auth.KindUser)},
							},
							{
								Key:      "resource.tenant",
								Operator: "equals",
								Values:   []string{globals.DefaultTenant},
							},
							{
								Key:      "resource.name",
								Operator: "equals",
								Values:   []string{ts.tu.User},
							},
						},
					},
				},
				From:       0,
				MaxResults: 50,
				Aggregate:  true,
			}
			Eventually(func() error {
				resp := testutils.AuditSearchResponse{}
				err := ts.tu.Search(ts.loggedInCtx, query, &resp)
				if err != nil {
					return err
				}
				if resp.ActualHits == 0 {
					return fmt.Errorf("no audit logs for [%s|%s] successful login", globals.DefaultTenant, ts.tu.User)
				}
				events := resp.AggregatedEntries.Tenants[globals.DefaultTenant].Categories[globals.Kind2Category("AuditEvent")].Kinds[auth.Permission_AuditEvent.String()].Entries
				for _, event := range events {
					if (event.Object.Action == svc.LoginAction) &&
						(event.Object.Outcome == audit.Outcome_Success.String()) &&
						(event.Object.User.Name == ts.tu.User) &&
						(event.Object.User.Tenant == globals.DefaultTenant) {
						return nil
					}
				}
				return fmt.Errorf("no audit logs for [%s|%s] successful login", globals.DefaultTenant, ts.tu.User)
			}, 30, 1).Should(BeNil())
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// check audit logs for Debug action
			query = &search.SearchRequest{
				Query: &search.SearchQuery{
					Kinds: []string{auth.Permission_AuditEvent.String()},
					Fields: &fields.Selector{
						Requirements: []*fields.Requirement{
							{
								Key:      "action",
								Operator: "equals",
								Values:   []string{"Debug"},
							},
							{
								Key:      "outcome",
								Operator: "equals",
								Values:   []string{audit.Outcome_Success.String()},
							},
							{
								Key:      "resource.kind",
								Operator: "equals",
								Values:   []string{string(diagnostics.KindModule)},
							},
							{
								Key:      "resource.name",
								Operator: "equals",
								Values:   []string{updatedModObj.Name},
							},
						},
					},
				},
				From:       0,
				MaxResults: 50,
				Aggregate:  true,
			}
			Eventually(func() error {
				resp := testutils.AuditSearchResponse{}
				err := ts.tu.Search(ts.loggedInCtx, query, &resp)
				if err != nil {
					return err
				}
				if resp.ActualHits == 0 {
					return fmt.Errorf("no audit logs for [%s] Debug action", updatedModObj.Name)
				}
				events := resp.AggregatedEntries.Tenants[globals.DefaultTenant].Categories[globals.Kind2Category("AuditEvent")].Kinds[auth.Permission_AuditEvent.String()].Entries
				for _, event := range events {
					if (event.Object.Action == "Debug") &&
						(event.Object.Outcome == audit.Outcome_Success.String()) &&
						(event.Object.User.Name == ts.tu.User) &&
						(event.Object.User.Tenant == globals.DefaultTenant) {
						return nil
					}
				}
				return fmt.Errorf("no audit logs for [%s] Debug action", updatedModObj.Name)
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("npm", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.Npm)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.Npm)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
		It("check Action (list-objects) query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Action.String(),
					Parameters: map[string]string{"action": "list-objects", "kind": "Network"},
				}, &resp); err != nil {
					return err
				}
				if strings.Contains(respStr, "unknown query") ||
					strings.Contains(respStr, "rpc error:") {
					return fmt.Errorf("did not succeed: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
		It("check Action (dump-nimbus-db) query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Action.String(),
					Parameters: map[string]string{"action": "dump-nimbus-db"},
				}, &resp); err != nil {
					return err
				}
				if strings.Contains(respStr, "unknown query") ||
					strings.Contains(respStr, "rpc error:") {
					return fmt.Errorf("did not succeed: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("apigw logs", func() {
		var modObj *diagnostics.Module
		var restSvc apiclient.Services
		var node string
		BeforeEach(func() {
			var err error
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.APIGw)
				restSvc, err = apiclient.NewRestAPIClient(node)
				if err != nil {
					return err
				}
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.APIGw)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			Eventually(func() error {
				// create debug logs by sending a request to APIGw on a particular node
				if _, err := restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, modObj.GetObjectMeta()); err != nil {
					return err
				}
				// query logs through Debug action
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.DebugOnAPIGw(ts.loggedInCtx, node, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("citadel", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.Citadel)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.Citadel)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("evtsmgr", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.EvtsMgr)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.EvtsMgr)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("rollout", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.Rollout)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.Rollout)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("tpm", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.Tpm)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.Tpm)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check log query", func() {
			modObj.Spec.LogLevel = diagnostics.ModuleSpec_Debug.String()
			var updatedModObj *diagnostics.Module
			var err error
			Eventually(func() error {
				updatedModObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, modObj)
				return err
			}, 10, 1).Should(BeNil())
			Expect(modObj.Spec.LogLevel).Should(Equal(diagnostics.ModuleSpec_Debug.String()))
			// query logs through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: updatedModObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Log.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"level\":\"debug\"") &&
					!strings.Contains(respStr, "\"level\":\"info\"") &&
					!strings.Contains(respStr, "\"level\":\"error\"") {
					return fmt.Errorf("no logs returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
			// restore info log level
			Eventually(func() error {
				updatedModObj.Spec.LogLevel = diagnostics.ModuleSpec_Info.String()
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Update(ts.loggedInCtx, updatedModObj)
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("naples", func() {
		var modObjs []*diagnostics.Module
		BeforeEach(func() {
			var err error
			Eventually(func() error {
				modObjs, err = ts.restSvc.DiagnosticsV1().Module().List(ts.loggedInCtx, &api.ListWatchOptions{
					FieldSelector: fmt.Sprintf("status.category=%s", diagnostics.ModuleStatus_Naples.String())})
				if err != nil {
					return err
				}
				if len(modObjs) == 0 {
					return errors.New("no module objects found for Naples")
				}
				return err
			}, 60, 1).Should(BeNil())
		})
		It("check query", func() {
			var err error
			// query through Debug action
			Eventually(func() error {
				resp := make(map[string]interface{})
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObjs[0].Name},
					Parameters: map[string]string{
						"command": "ps",
					},
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "PID") {
					return fmt.Errorf("no query response returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
	Context("apiserver", func() {
		var modObj *diagnostics.Module
		BeforeEach(func() {
			var err error
			var node string
			Eventually(func() error {
				node = ts.tu.GetNodeForService(globals.APIServer)
				modObj, err = ts.restSvc.DiagnosticsV1().Module().Get(ts.loggedInCtx, &api.ObjectMeta{Name: fmt.Sprintf("%s-%s", node, globals.APIServer)})
				return err
			}, 10, 1).Should(BeNil())
		})
		It("check stats query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Stats.String(),
				}, &resp); err != nil {
					return err
				}
				if !strings.Contains(respStr, "\"cmdline\":") ||
					!strings.Contains(respStr, "\"cpustats\":") ||
					!strings.Contains(respStr, "\"memstats\":") {
					return fmt.Errorf("no stats returned: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
		It("check Action (list-watchers) query", func() {
			var err error
			// query stats through Debug action
			Eventually(func() error {
				type debugResponse struct {
					Diagnostics map[string]interface{} `json:"diagnostics"`
				}
				resp := debugResponse{}
				var respStr string
				if respStr, err = ts.tu.Debug(ts.loggedInCtx, &diagnostics.DiagnosticsRequest{
					ObjectMeta: api.ObjectMeta{Name: modObj.Name},
					Query:      diagnostics.DiagnosticsRequest_Action.String(),
					Parameters: map[string]string{"action": "list-watchers"},
				}, &resp); err != nil {
					return err
				}
				if strings.Contains(respStr, "unknown query") ||
					strings.Contains(respStr, "rpc error:") {
					return fmt.Errorf("did not succeed: {%v}", respStr)
				}
				return nil
			}, 30, 1).Should(BeNil())
		})
	})
})
