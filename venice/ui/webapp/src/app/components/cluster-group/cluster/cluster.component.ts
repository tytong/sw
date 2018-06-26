import { Component, OnInit, ViewChild, ViewEncapsulation } from '@angular/core';
import { ControllerService } from '@app/services/controller.service';
import { ClusterService } from '@app/services/generated/cluster.service';
import { BaseComponent } from '../../base/base.component';
import { Eventtypes } from '@app/enum/eventtypes.enum';
import {Table} from 'primeng/table';
import { ApiStatus, ClusterCluster, ClusterClusterList, ClusterNode, ClusterNodeList } from '@sdk/v1/models/generated/cluster';

@Component({
  selector: 'app-cluster',
  encapsulation: ViewEncapsulation.None,
  templateUrl: './cluster.component.html',
  styleUrls: ['./cluster.component.scss']
})
export class ClusterComponent extends BaseComponent implements OnInit {
  @ViewChild('nodestable') nodesTable: Table;

  bodyicon: any = {
    margin: {
      top: '9px',
      left: '8px'
    },
    url: '/assets/images/icons/cluster/ico-cluster-black.svg'
  };
  cluster: ClusterCluster;
  nodes: ClusterNode[] = [];
  nodeCount: Number = 0;
  cols: any[] = [
    { field: 'name', header: 'Name' },
    { field: 'quorum', header: 'Quorum Member' },
    { field: 'phase', header: 'Phase' },
  ];

  constructor(
    private _clusterService: ClusterService,
    protected _controllerService: ControllerService,
  ) {
    super(_controllerService);
  }

  ngOnInit() {
    if (!this._controllerService.isUserLogin()) {
      this._controllerService.publish(Eventtypes.NOT_YET_LOGIN, {});
    } else {
      this.getCluster();
      this.getNodes();

      this._controllerService.setToolbarData({
        buttons: [
          {
            cssClass: 'global-button-primary cluster-toolbar-button',
            text: 'Refresh',
            callback: () => { this.getCluster(); this.getNodes(); },
          }],
        breadcrumb: [{ label: 'Cluster', url: ''}, {label: 'Cluster', url: ''}]
      });
    }
  }

  getCluster() {
    this._clusterService.ListCluster().subscribe(
      data => {
        if (data.statusCode !== 200) {
          console.log('Cluster service returned code: ' + data.statusCode + ' data: ' + <ApiStatus>data.body);
          // TODO: Error handling
          return;
        }
        const clusters: ClusterClusterList = <ClusterClusterList>data.body;

        if (clusters.Items.length > 0) {
          this.cluster = clusters.Items[0];
        }
      }
    );
  }

  getNodes() {
    this._clusterService.ListNode().subscribe(
      data => {
        if (data.statusCode !== 200) {
          console.log('Node service returned code: ' + data.statusCode + ' data: ' + <ApiStatus>data.body);
          // TODO: Error handling
          return;
        }
        const nodes: ClusterNodeList = <ClusterNodeList>data.body;

        this.nodeCount = nodes.Items.length;
        this.nodes = nodes.Items;
      }
    );
  }
}
