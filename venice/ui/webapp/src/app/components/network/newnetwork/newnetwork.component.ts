import { Component, OnInit, ViewEncapsulation, AfterViewInit, Input, ChangeDetectionStrategy } from '@angular/core';
import { ControllerService } from '@app/services/controller.service';
import { CreationForm } from '@app/components/shared/tableviewedit/tableviewedit.component';
import { Animations } from '@app/animations';
import { NetworkNetwork, INetworkNetwork, NetworkOrchestratorInfo } from '@sdk/v1/models/generated/network';
import { NetworkService } from '@app/services/generated/network.service';
import { OrchestrationService } from '@app/services/generated/orchestration.service';
import { UIConfigsService } from '@app/services/uiconfigs.service';
import { FormArray, ValidatorFn, AbstractControl, Validators, ValidationErrors, FormGroup } from '@angular/forms';
import { Utility } from '@app/common/Utility';
import { HttpEventUtility } from '@app/common/HttpEventUtility';
import { OrchestrationOrchestrator } from '@sdk/v1/models/generated/orchestration';
import { SelectItem } from 'primeng/api';
import { minValueValidator, maxValueValidator } from '@sdk/v1/utils/validators';
import { UIRolePermissions } from '@sdk/v1/models/generated/UI-permissions-enum';

@Component({
  selector: 'app-newnetwork',
  templateUrl: './newnetwork.component.html',
  styleUrls: ['./newnetwork.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: Animations,
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class NewnetworkComponent extends CreationForm<INetworkNetwork, NetworkNetwork> implements OnInit, AfterViewInit {

  @Input() existingObjects: INetworkNetwork[] = [];

  vcenterEventUtility: HttpEventUtility<OrchestrationOrchestrator>;
  vcenters: ReadonlyArray<OrchestrationOrchestrator> = [];
  vcenterOptions: SelectItem[] = [];

  createButtonTooltip: string = 'Ready to submit';

  constructor(protected _controllerService: ControllerService,
    protected uiconfigsService: UIConfigsService,
    protected orchestrationService: OrchestrationService,
    protected networkService: NetworkService,
  ) {
    super(_controllerService, uiconfigsService, NetworkNetwork);
  }

  getClassName() {
    return this.constructor.name;
  }

  getVcenterIntegrations() {
    this.vcenterEventUtility = new HttpEventUtility<OrchestrationOrchestrator>(OrchestrationOrchestrator);
    this.vcenters = this.vcenterEventUtility.array as ReadonlyArray<OrchestrationOrchestrator>;
    const sub = this.orchestrationService.WatchOrchestrator().subscribe(
      response => {
        this.vcenterEventUtility.processEvents(response);
        this.vcenterOptions = this.vcenters.map(vcenter => {
          return {
            label: vcenter.meta.name,
            value: vcenter.meta.name
          };
        });
        this.vcenterOptions.push({label: '', value: null});
      },
      this.controllerService.webSocketErrorHandler('Failed to get vCenter Integrations')
    );
    this.subscriptions.push(sub);
  }

  // Empty Hook
  postNgInit() {
    this.getVcenterIntegrations();

    this.newObject.$formGroup.get(['meta', 'name']).setValidators([
      this.newObject.$formGroup.get(['meta', 'name']).validator,
      this.isNewPolicyNameValid(this.existingObjects)]);

    this.newObject.$formGroup.get(['spec', 'vlan-id']).setValidators(
      [minValueValidator(0), maxValueValidator(65536)]);

    // Add one collectors if it doesn't already have one
    const collectors = this.newObject.$formGroup.get(['spec', 'orchestrators']) as FormArray;
    if (collectors.length === 0) {
      this.addOrchestrator();
    }
  }

  isNewPolicyNameValid(existingObjects: INetworkNetwork[]): ValidatorFn {
    // checks if name field is valid
    return Utility.isModelNameUniqueValidator(existingObjects, 'newNetwork-name');
  }

  addOrchestrator() {
    const orchestrators = this.newObject.$formGroup.get(['spec', 'orchestrators']) as FormArray;
    const newOrchestrator = new NetworkOrchestratorInfo().$formGroup;
    orchestrators.insert(orchestrators.length, newOrchestrator);
  }

  removeOrchestrator(index: number) {
    const orchestrators = this.newObject.$formGroup.get(['spec', 'orchestrators']) as FormArray;
    if (orchestrators.length > 1) {
      orchestrators.removeAt(index);
    }
  }

  // Empty Hook
  isFormValid() {
    if (Utility.isEmpty(this.newObject.$formGroup.get(['spec', 'vlan-id']).value)) {
      this.createButtonTooltip = 'Error: VLAN is required.';
      return false;
    }

    if (!this.newObject.$formGroup.get(['spec', 'vlan-id']).valid) {
      this.createButtonTooltip = 'Error: Invalid VLAN';
      return false;
    }

    const orchestrators = this.controlAsFormArray(
      this.newObject.$formGroup.get(['spec', 'orchestrators'])).controls;
    for (let i = 0; i < orchestrators.length; i++) {
      const orchestrator = orchestrators[i];
      if (Utility.isEmpty(orchestrator.get(['orchestrator-name']).value)) {
        this.createButtonTooltip = 'Error: VCenter name is required.';
        return false;
      }
      if (Utility.isEmpty(orchestrator.get(['namespace']).value)) {
        this.createButtonTooltip = 'Error: Datacenter name is required.';
        return false;
      }
    }

    this.createButtonTooltip = 'Ready to submit';
    return true;
  }

  getTooltip(): string {
    if (Utility.isEmpty(this.newObject.$formGroup.get(['meta', 'name']).value)) {
      return 'Error: Name field is empty.';
    }
    if (!this.newObject.$formGroup.get(['meta', 'name']).valid)  {
      return 'Error: Name field is invalid.';
    }
    return this.createButtonTooltip;
  }

  setToolbar() {
    if (!this.isInline && this.uiconfigsService.isAuthorized(UIRolePermissions.networknetwork_create)) {
      const currToolbar = this.controllerService.getToolbarData();
      currToolbar.buttons = [
        {
          cssClass: 'global-button-primary global-button-padding',
          text: 'CREATE NETWORK',
          callback: () => { this.saveObject(); },
          computeClass: () => this.computeButtonClass(),
          genTooltip: () => this.getTooltip(),
        },
        {
          cssClass: 'global-button-neutral global-button-padding',
          text: 'CANCEL',
          callback: () => { this.cancelObject(); }
        },
      ];

      this._controllerService.setToolbarData(currToolbar);
    }
  }

  createObject(object: INetworkNetwork) {
    return this.networkService.AddNetwork(object);
  }

  updateObject(newObject: INetworkNetwork, oldObject: INetworkNetwork) {
    return this.networkService.UpdateNetwork(oldObject.meta.name, newObject, null, oldObject);
  }

  generateCreateSuccessMsg(object: INetworkNetwork) {
    return 'Created network ' + object.meta.name;
  }

  generateUpdateSuccessMsg(object: INetworkNetwork) {
    return 'Updated network ' + object.meta.name;
  }

  isFieldEmpty(field: AbstractControl): boolean {
    return Utility.isEmpty(field.value);
  }

}
